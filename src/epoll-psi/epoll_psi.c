#include "mq.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 4
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

        int fds[NUM_RESOURCES];
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

        for (int i = 0; i < NUM_RESOURCES; i++) {
                char path[256];
                snprintf(path, sizeof(path), "/proc/pressure/%s", resources[i]);
                fds[i] = open(path, O_RDONLY | O_NONBLOCK);
                if (fds[i] == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                }

                event.events = EPOLLIN;
                event.data.fd = fds[i];
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[i], &event) == -1) {
                        perror("epoll_ctl");
                        exit(EXIT_FAILURE);
                }
        }

        printf("Entering main loop with duty cycle of %d seconds\n", duty_cycle);

        if (init_mq(MQ_PATH) == -1)
                return 1;

        char message_batch[MESSAGE_BATCH_SIZE][BUFFER_SIZE];
        int batch_index = 0;
        time_t last_read_time = 0;

        while (1) {
                int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
                if (n == -1) {
                        perror("epoll_wait");
                        exit(EXIT_FAILURE);
                }

                time_t current_time = time(NULL);
                if (current_time - last_read_time >= duty_cycle) {
                        for (int j = 0; j < NUM_RESOURCES; j++) {
                                char buf[BUFFER_SIZE];
                                if (lseek(fds[j], 0, SEEK_SET) == -1) {
                                        perror("lseek");
                                        continue;
                                }
                                ssize_t count = read(fds[j], buf, BUFFER_SIZE - 1);
                                if (count == -1) {
                                        perror("read");
                                        continue;
                                }

                                if (count > 0) {
                                        buf[count] = '\0';
                                        process_psi_data(
                                                        buf,
                                                        resources[j],
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

        for (int i = 0; i < NUM_RESOURCES; i++) {
                close(fds[i]);
        }

        if (timer_fd != -1) {
                close(timer_fd);
        }

        return 0;
}