#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>        /* For O_* constants */
#include <sys/stat.h>     /* For mode constants */
#include <linux/limits.h> /* For NAME_MAX */
#include "inotify-coredump.h"

#define WATCH_DIR "/var/lib/systemd/coredump"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */
// TODO change following macros according to main.c (posix-mq-to-mqtt-bridge)
#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 150

/* Send message in InfluxDB Line Protocol to posix-mq-to-mqtt-bridge */
int send_to_mq(const char *message)
{
    //! Test
    printf("send_to_mq: %s\n", message);
    //! mqd_t mq = mq_open(MQ_PATH, O_WRONLY | O_CREAT, 0666, NULL);

    mqd_t mq = mq_open(MQ_PATH, O_WRONLY);
    if (mq == -1)
    {
        return -1;
    }

    if (mq_send(mq, message, strlen(message), 0) == -1)
    {
        mq_close(mq);
        return -1;
    }
    mq_close(mq);
    return 0;
}

int inotify_coredump()
{
    int fd, wd, len;
    char buffer[BUF_LEN];

    /* Initialize inotify instance */
    fd = inotify_init();
    if (fd < 0)
    {
        perror("inotify_init failed");
        return -1;
    }

    /* Watch WATCH_DIR for new files */
    wd = inotify_add_watch(fd, WATCH_DIR, IN_CREATE);
    if (wd < 0)
    {
        perror("inotify_add_watch failed");
        close(fd);
        return -1;
    }

    /* Event loop */
    for (;;)
    {
        /* Read events */
        len = read(fd, buffer, BUF_LEN);
        if (len < 0)
        {
            perror("read failed");
            break;
        }

        /* Process events */
        for (int i = 0; i < len;)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len && event->mask & IN_CREATE)
            {
                char message[MAX_MSG_SIZE];
                snprintf(message, MAX_MSG_SIZE + 1, "New core dump file created in %s: %s\n", WATCH_DIR, event->name);

                //! Test
                printf("%s\n", message);

                if (send_to_mq(message) == -1)
                {
                    perror("send_to_mq failed");
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}

int main()
{
    return inotify_coredump();
}