#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>        /* For O_* constants */
#include <sys/stat.h>     /* For mode constants */
#include <linux/limits.h> /* For NAME_MAX */

#define WATCH_DIR "/var/lib/systemd/coredump"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */
// TODO change following macros according to main.c (posix-mq-to-mqtt-bridge)
#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 50

/* Send message in InfluxDB Line Protocol to posix-mq-to-mqtt-bridge */
void send_to_mq(const char *message)
{
    //! Test
    printf("%s\n", message);

    mqd_t mq = mq_open(MQ_PATH, O_WRONLY);
    if (mq == -1)
    {
        perror("mq_open failed");
        exit(-1);
    }

    if (mq_send(mq, message, strlen(message), 0) == -1)
    {
        perror("mq_send failed");
        //! mq_close(mq);
        exit(-1);
    }
}

int main()
{
    int fd, wd, len;
    char buf[BUF_LEN];

    /* Initialize inotify instance */
    fd = inotify_init();
    if (fd < 0)
    {
        perror("inotify_init failed");
        return 1;
    }

    /* Watch WATCH_DIR for new files */
    wd = inotify_add_watch(fd, WATCH_DIR, IN_CREATE);
    if (wd < 0)
    {
        perror("inotify_add_watch failed");
        close(fd);
        return 1;
    }

    /* Event loop */
    for (;;)
    {
        int i = 0;

        /* Read events */
        len = read(fd, buf, BUF_LEN);
        if (len < 0)
        {
            perror("read failed");
            break;
        }

        /* Process events */
        while (i < len)
        {
            struct inotify_event *event = (struct inotify_event *)&buf[i];
            if (event->len && event->mask & IN_CREATE)
            {
                char message[MAX_MSG_SIZE];
                snprintf(message, MAX_MSG_SIZE, "New core dump file created: %s\n", event->name);

                //! Test
                printf("%s\n", message);

                send_to_mq(message);
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    exit(EXIT_SUCCESS);

    return 0;
}