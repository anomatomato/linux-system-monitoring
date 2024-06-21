#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 1
#define BUFFER_SIZE 1024
#define MQ_PATH "/my_queue"

struct psi_values
{
    double avg10;
    double avg60;
    double avg300;
    long total;
};

void send_to_mq(const char* message)
{
    printf("%s\n", message);

    mqd_t mq = mq_open(MQ_PATH, O_WRONLY);
    if (mq == -1)
    {
        perror("mq_open failed");
        exit(EXIT_FAILURE);
    }

    if (mq_send(mq, message, strlen(message), 0) == -1)
    {
        perror("mq_send failed");
        mq_close(mq);
        exit(EXIT_FAILURE);
    }

    mq_close(mq);
}

void process_psi_data(char* data)
{
    struct psi_values psi;
    char formatted_message[BUFFER_SIZE];
    char* token;
    const char delim[2] = " ";

    token = strtok(data, delim);
    while (token != NULL)
    {
        if (strncmp(token, "avg10=", 6) == 0)
        {
            psi.avg10 = atof(token + 6);
        }
        else if (strncmp(token, "avg60=", 6) == 0)
        {
            psi.avg60 = atof(token + 6);
        }
        else if (strncmp(token, "avg300=", 7) == 0)
        {
            psi.avg300 = atof(token + 7);
        }
        else if (strncmp(token, "total=", 6) == 0)
        {
            psi.total = atol(token + 6);
        }
        token = strtok(NULL, delim);
    }

    snprintf(
        formatted_message, BUFFER_SIZE,
        "psi,resource=cpu,stalled=some avg10=%.2f,avg60=%.2f,avg300=%.2f,total=%ld 
            % ld\n ",
              psi.avg10,
        psi.avg60, psi.avg300, psi.total, (long int)time(NULL));
    send_to_mq(formatted_message);
}

int main()
{
    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    int fd = open("/proc/pressure/cpu", O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events  = EPOLLPRI;
    event.data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        perror("epoll_ctl");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char buf[BUFFER_SIZE];
    while (1)
    {
        struct epoll_event events[MAX_EVENTS];
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n == -1)
        {
            perror("epoll_wait");
            close(fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++)
        {
            if (events[i].events & EPOLLPRI)
            {
                lseek(fd, 0, SEEK_SET);
                ssize_t count = read(fd, buf, BUFFER_SIZE - 1);
                if (count == -1)
                {
                    perror("read");
                    close(fd);
                    exit(EXIT_FAILURE);
                }

                if (count > 0)
                {
                    buf[count] = '\0';
                    process_psi_data(buf);
                }
            }
        }
    }

    close(fd);
    return 0;
}
