#include "mq.h"
#include <bits/getopt_core.h>
#include <dirent.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#define MAX_EVENTS 30
#define BUFFER_SIZE 1024
#define MQ_PATH "/psi"
#define NUM_RESOURCES 3
#define DEFAULT_DUTY_CYCLE 0
#define TFD_CLOCK CLOCK_MONOTONIC
#define MESSAGE_BATCH_SIZE 10

const char *resources[NUM_RESOURCES] = { "cpu", "io", "memory" };

struct psi_values {
        double avg10, avg60, avg300;
        long total;
};

void process_psi_data(char *data, const char *resource, char *output, size_t output_size) {
        struct psi_values psi = { 0 };
        sscanf(data,
               "some avg10=%lf avg60=%lf avg300=%lf total=%ld",
               &psi.avg10,
               &psi.avg60,
               &psi.avg300,
               &psi.total);

        snprintf(output,
                 output_size,
                 "psi,resource=%s avg10=%.2f,avg60=%.2f,avg300=%.2f,total=%ld %ld",
                 resource,
                 psi.avg10,
                 psi.avg60,
                 psi.avg300,
                 psi.total,
                 (long int) time(NULL));
}

int create_timer_fd(int sec) {
        int timerfd = timerfd_create(TFD_CLOCK, 0);
        if (timerfd == -1) {
                perror("timerfd_create");
                exit(EXIT_FAILURE);
        }

        struct itimerspec new_value = {
                .it_value = { .tv_sec = sec, .tv_nsec = 0 },
                  .it_interval = { .tv_sec = sec, .tv_nsec = 0 }
        };

        if (timerfd_settime(timerfd, 0, &new_value, NULL) == -1) {
                perror("timerfd_settime");
                exit(EXIT_FAILURE);
        }

        return timerfd;
}


void find_directories(char **dirs, const int dirs_max_size, const char *path) {
        struct dirent *entry;

        struct stat statbuf = {};
        DIR *dp = opendir(path);

        if (dp == NULL) {
                perror("opendir");
                return;
        }
        int i = 0;
        while ((entry = readdir(dp)) != NULL && i < dirs_max_size) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                        continue;

                char full_path[MAX_MSG_SIZE];
                snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

                if (stat(full_path, &statbuf) == -1) {
                        perror("stat");
                        continue;
                }

                if (S_ISDIR(statbuf.st_mode)) {
                        dirs[i] = malloc(MAX_MSG_SIZE);
                        if (dirs[i] == NULL) {
                                perror("malloc");
                                break;
                        }
                        snprintf(dirs[i], MAX_MSG_SIZE, "%s/", full_path);
                        printf("%s\n", dirs[i]);
                        i++;
                }
        }

        closedir(dp);
}

int *register_files_in_dir(char *dir_name, int epfd) {
        for (int i = 0; i < NUM_RESOURCES; i++) {
                struct epoll_event event;
                char path[256];
                strcpy(path, dir_name);
                strcat(path, resources[i]);
                // snprintf(path, sizeof(path), "/proc/pressure/%s", resources[i]);
                if (strstr(dir_name, "/sys/fs/cgroup") != NULL)
                        strcat(path, ".pressure");

                printf("path:%s\n, dir:%s", path, dir_name);
                int fd = open(path, O_RDONLY | O_NONBLOCK);
                if (fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                }

                event.events = EPOLLIN;
                event.data.fd = fd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) {
                        perror("epoll_ctl");
                        exit(EXIT_FAILURE);
                }
        }
}

int main(int argc, char *argv[]) {
        int duty_cycle = DEFAULT_DUTY_CYCLE;
        int verbose = 0;

        int opt;
        while ((opt = getopt(argc, argv, "c:v")) != -1) {
                switch (opt) {
                case 'c':
                        duty_cycle = atoi(optarg);
                        break;
                case 'v':
                        verbose = 1;
                        break;
                default:
                        fprintf(stderr, "Usage: %s [-c duty_cycle] [-v]\n", argv[0]);
                        exit(EXIT_FAILURE);
                }
        }

        int epfd = epoll_create1(0);
        if (epfd == -1) {
                perror("epoll_create1");
                exit(EXIT_FAILURE);
        }

        const int max_dirs = 9;
        struct epoll_event event, events[MAX_EVENTS];

        int timer_fd = -1;
        if (duty_cycle > 0) {
                timer_fd = create_timer_fd(duty_cycle);
                event.events = EPOLLIN;
                event.data.fd = timer_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, timer_fd, &event) == -1) {
                        perror("epoll_ctl");
                        exit(EXIT_FAILURE);
                }
        }
        register_files_in_dir("/proc/pressure/", epfd);
        // char* dirs[max_dirs];
        // find_directories(dirs, max_dirs, "/sys/fs/cgroup");
        // for(int i = 0; i < max_dirs; i++) {
        //         if (dirs[i] == NULL)
        //                 break;
        //         register_files_in_dir(fds, &event, dirs[i], epfd);
        //         free(dirs[i]);
        // }
        printf("Entering main loop with duty cycle of %d seconds\n", duty_cycle);

        if (init_mq(MQ_PATH) == -1)
                return 1;

        char message_batch[MESSAGE_BATCH_SIZE][BUFFER_SIZE];
        int batch_index = 0;
        time_t last_read_time = 0;

        while (1) {
                int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
                if (nfds == -1) {
                        perror("epoll_wait");
                        exit(EXIT_FAILURE);
                }

                time_t current_time = time(NULL);
                if (current_time - last_read_time >= duty_cycle) {
                        for (int n = 0; n < nfds; ++n) {
                                char buf[BUFFER_SIZE];
                                int fd = events[n].data.fd;
                                printf("%d\n", fd);
                                if (lseek(fd, 0, SEEK_SET) == -1) {
                                        perror("lseek");
                                        continue;
                                }
                                ssize_t count = read(fd, buf, BUFFER_SIZE - 1);
                                if (count == -1) {
                                        perror("read");
                                        continue;
                                }

                                if (count > 0) {
                                        buf[count] = '\0';
                                        process_psi_data(
                                                        buf,
                                                        resources[n],
                                                        message_batch[batch_index],
                                                        BUFFER_SIZE);
                                        if (verbose) {
                                                printf("%s\n", message_batch[batch_index]);
                                        }
                                        batch_index++;

                                        if (batch_index == MESSAGE_BATCH_SIZE) {
                                                for (int k = 0; k < MESSAGE_BATCH_SIZE; k++) {
                                                        send_to_mq(message_batch[k], MQ_PATH);
                                                }
                                                batch_index = 0;
                                        }
                                }
                        }
                        last_read_time = current_time;
                }

                struct timespec sleep_time = { 0, 1000000 }; // 1 millisecond
                nanosleep(&sleep_time, NULL);
        }

        if (timer_fd != -1)
                close(timer_fd);

        return 0;
}