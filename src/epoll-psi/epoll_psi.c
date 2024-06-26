#include <fcntl.h>
#include "../utilities/mq.h"    /* Include the message queue utility for inter-process communication */
#include <mqueue.h>             /* Include POSIX message queue headers */
#include <stdio.h>              /* Standard I/O operations */
#include <stdlib.h>             /* Standard library for memory allocation, process control, conversions, etc */
#include <string.h>             /* String operations like strtok, strncpy, etc */
#include <sys/epoll.h>          /* Epoll event-driven I/O for handling large numbers of file descriptors */
#include <time.h>               /* Time functions */
#include <unistd.h>             /* POSIX operating system API */

#define MAX_EVENTS 1            /* Maximum number of events to be processed at once by epoll_wait */
#define BUFFER_SIZE 1024        /* Buffer size for reading data */
#define MQ_PATH "/my_queue"     /* Path to the POSIX message queue */

/* Structure to hold parsed PSI (Pressure Stall Information) values */
struct psi_values {
    double avg10;   /* Average pressure over the last 10 seconds */
    double avg60;   /* Average pressure over the last 60 seconds */
    double avg300;  /* Average pressure over the last 300 seconds */
    long total;     /* Total number of events */
};

/* Function to process the PSI data string and send it to a message queue */
void process_psi_data(char* data) {
    struct psi_values psi;      /* Struct to hold the PSI data */
    char formatted_message[BUFFER_SIZE];  /* Buffer to hold the formatted message */
    char* token;                /* Token pointer for strtok */
    const char delim[2] = " ";  /* Delimiter for tokenizing the data string */

    printf("Processing PSI data: %s\n", data);  /* Debug: Print raw data */

    /* Tokenize the input data string */
    token = strtok(data, delim);
    while (token != NULL) {
        if (strncmp(token, "avg10=", 6) == 0) {
            psi.avg10 = atof(token + 6);  /* Parse avg10 value */
        } else if (strncmp(token, "avg60=", 6) == 0) {
            psi.avg60 = atof(token + 6);  /* Parse avg60 value */
        } else if (strncmp(token, "avg300=", 7) == 0) {
            psi.avg300 = atof(token + 7); /* Parse avg300 value */
        } else if (strncmp(token, "total=", 6) == 0) {
            psi.total = atol(token + 6);  /* Parse total value */
        }
        token = strtok(NULL, delim);     /* Continue tokenizing the data string */
    }

    /* Print the parsed PSI values for debugging */
    printf("Parsed PSI values: avg10=%.2f, avg60=%.2f, avg300=%.2f, total=%ld\n", psi.avg10, psi.avg60, psi.avg300, psi.total);

    /* Format the message to be sent to the message queue */
    snprintf(
        formatted_message, BUFFER_SIZE,
        "psi,resource=cpu,stalled=some avg10=%.2f,avg60=%.2f,avg300=%.2f,total=%ld %ld\n",
        psi.avg10, psi.avg60, psi.avg300, psi.total, (long int)time(NULL)
    );
    send_to_mq(formatted_message, MQ_PATH);  /* Send the formatted message to the message queue */
}

int main() {
    printf("Starting epoll_psi program\n");  /* Debug: Indicate program start */

    int epfd = epoll_create1(0);  /* Create an epoll file descriptor */
    if (epfd == -1) {
        perror("epoll_create1");  /* Print an error message if epoll creation failed */
        exit(EXIT_FAILURE);       /* Exit with a failure result */
    }

    int fd = open("/proc/pressure/cpu", O_RDONLY);  /* Open the CPU pressure info file */
    if (fd == -1) {
        perror("open");           /* Print an error message if file opening failed */
        exit(EXIT_FAILURE);       /* Exit with a failure result */
    }

    struct epoll_event event;     /* Event structure for use with epoll */
    memset(&event, 0, sizeof(event));  /* Zero out the event structure */
    event.events  = EPOLLPRI;     /* Set the type of events to check for */
    event.data.fd = fd;           /* Set the file descriptor for events */

    /* Add the file descriptor to the epoll instance */
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");      /* Print an error message if epoll control operation failed */
        close(fd);                /* Close the file descriptor */
        exit(EXIT_FAILURE);       /* Exit with a failure result */
    }

    printf("Entering main event loop\n");  /* Debug: Indicate entering the main loop */

    char buf[BUFFER_SIZE];        /* Buffer for reading data from the file */
    while (1) {                   /* Infinite loop to handle events as they come */
        struct epoll_event events[MAX_EVENTS];  /* Array to hold events from epoll_wait */
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);  /* Wait for events on the epoll file descriptor */
        if (n == -1) {
            perror("epoll_wait"); /* Print an error message if epoll wait failed */
            close(fd);            /* Close the file descriptor */
            exit(EXIT_FAILURE);   /* Exit with a failure result */
        }

        printf("Event received: %d\n", n);  /* Debug: Indicate event received */

        /* Process each event */
        for (int i = 0; i < n; i++) {
            if (events[i].events & EPOLLPRI) {  /* Check for urgent data (high priority) */
                lseek(fd, 0, SEEK_SET);         /* Seek to the beginning of the file */
                ssize_t count = read(fd, buf, BUFFER_SIZE - 1);  /* Read from the file */
                if (count == -1) {
                    perror("read");             /* Print an error message if reading failed */
                    close(fd);                  /* Close the file descriptor */
                    exit(EXIT_FAILURE);         /* Exit with a failure result */
                }

                if (count > 0) {
                    buf[count] = '\0';          /* Null-terminate the buffer */
                    process_psi_data(buf);      /* Process the PSI data */
                }
            }
        }
    }

    close(fd);  /* Close the file descriptor */
    return 0;   /* Return success */
}
