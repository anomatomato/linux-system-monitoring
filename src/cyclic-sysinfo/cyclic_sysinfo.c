#include <fcntl.h> /* For O_* constants, like O_RDONLY used in open syscall */
#include "../utilities/mq.h" /* Custom utility for managing message queues */
#include <stdio.h> /* Standard input-output header for functions like printf, snprintf, perror */
#include <stdlib.h> /* Standard library for memory allocation, process control, and conversions */
#include <string.h> /* String handling functions like strtok and strncpy */
#include <sys/stat.h> /* For mode constants and file permissions */
#ifdef __linux__
#include <sys/sysinfo.h> /* Include sysinfo struct and sysinfo function declarations */
#else
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#include <unistd.h> /* For POSIX constants and functions like sleep, close, and read */
#include <time.h> /* For clock and timer functions */
#include <sys/timerfd.h> /* For timer file descriptor functions and constants */
#include <sys/epoll.h> /* For epoll functions and constants */
#include <mqueue.h> /* For POSIX message queues */

#define MQ_PATH "/sysinfo" /* Path for the message queue */
#define CYCLE_INTERVAL 5 /* Interval in seconds to periodically gather system info */
#define MAX_EVENTS 10 /* Max number of events the epoll instance can report in one go */

void gather_sysinfo() {
#ifdef __linux__
    struct sysinfo info; /* Structure to hold system statistics */
    if (sysinfo(&info) == 0) { /* Successfully gathered system info */
        char message[256]; /* Buffer to hold the message to be sent */
        /* Format system information into InfluxDB Line Protocol */
        snprintf(message, sizeof(message),
                 "sysinfo,host=my_host uptime=%ld,total_ram=%lu,free_ram=%lu,process_count=%hu %ld",
                 info.uptime, /* System uptime in seconds */
                 info.totalram * info.mem_unit, /* Total usable main memory size */
                 info.freeram * info.mem_unit, /* Available memory size */
                 info.procs, /* Number of current processes */
                 (long)time(NULL) * 1000000000); /* Timestamp in nanoseconds */
        printf("Stats: %s\n", message); /* Print the gathered information for demonstration */

        /* Send message to the message queue */
        send_to_mq(message, MQ_PATH);
    } else {
        perror("sysinfo call failed"); /* Print an error if sysinfo call fails */
    }
#else
    int mib[2];
    int64_t physical_memory;
    size_t length = sizeof(physical_memory);

    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;

    if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0) {
        char message[256]; /* Buffer to hold the message to be sent */
        /* Format system information into InfluxDB Line Protocol */
        snprintf(message, sizeof(message),
                 "sysinfo,host=my_host total_ram=%lld %ld",
                 physical_memory, /* Total usable main memory size */
                 (long)time(NULL) * 1000000000); /* Timestamp in nanoseconds */
        printf("Stats: %s\n", message); /* Print the gathered information for demonstration */

        /* Send message to the message queue */
        send_to_mq(message, MQ_PATH);
    } else {
        perror("sysctl call failed"); /* Print an error if sysctl call fails */
    }
#endif
}

int main() {
    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1) {
        perror("timerfd_create failed"); /* Handle error if timer creation fails */
        exit(EXIT_FAILURE); /* Exit with a failure result */
    }

    struct itimerspec timerValue;
    timerValue.it_value.tv_sec = CYCLE_INTERVAL; /* Initial expiration after CYCLE_INTERVAL seconds */
    timerValue.it_value.tv_nsec = 0; /* No initial expiration nanoseconds */
    timerValue.it_interval.tv_sec = CYCLE_INTERVAL; /* Timer interval in seconds */
    timerValue.it_interval.tv_nsec = 0; /* Timer interval in nanoseconds */

    if (timerfd_settime(fd, 0, &timerValue, NULL) == -1) {
        perror("timerfd_settime failed"); /* Handle error if setting timer fails */
        close(fd); /* Close the timer file descriptor */
        exit(EXIT_FAILURE); /* Exit with a failure result */
    }

    int epoll_fd = epoll_create1(0); /* No flags are specified (0), using default settings */
    if (epoll_fd == -1) {
        perror("epoll_create failed"); /* Handle error if epoll creation fails */
        close(fd); /* Close the timer file descriptor */
        exit(EXIT_FAILURE); /* Exit with a failure result */
    }

    struct epoll_event ev;
    ev.events = EPOLLIN; /* Interested in the input-ready event (EPOLLIN) */
    ev.data.fd = fd; /* Associate the timer file descriptor with the event */

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl failed"); /* Handle error if epoll control operation fails */
        close(epoll_fd); /* Close the epoll file descriptor */
        close(fd); /* Close the timer file descriptor */
        exit(EXIT_FAILURE); /* Exit with a failure result */
    }
    if (init_mq(MQ_PATH) == -1) /*mq für die bridge erstellen*/
            return 1;
    struct epoll_event events[MAX_EVENTS];
    while (1) { /* Infinite loop to handle events as they come */
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait failed"); /* Handle error if epoll wait fails */
            break; /* Break the loop to exit */
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == fd) { /* Check if the event is from the timer */
                uint64_t expirations; /* Variable to count how many times the timer has expired */
                read(fd, &expirations, sizeof(expirations)); /* Read to clear the event */
                gather_sysinfo(); /* Call gather_sysinfo function to gather and print system info */
            }
        }
    }

    close(epoll_fd); /* Close the epoll file descriptor */
    close(fd); /* Close the timer file descriptor */
    return 0; /* Return success */
}
