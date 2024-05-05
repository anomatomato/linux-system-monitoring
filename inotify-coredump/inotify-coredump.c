#include "inotify-coredump.h"
#include "../utilities/mq.h"
#include <fcntl.h>        /* For O_* constants */
#include <linux/limits.h> /* For NAME_MAX */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

#define WATCH_DIR "/var/lib/systemd/coredump"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN                                                                \
    (1024 *                                                                    \
     (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

int inotify_coredump()
{
    int fd, wd, len;
    char buffer[BUF_LEN];
    printf("I am running in inotify_coredump\n");
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
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            printf("looping\n");
            if (event->len && event->mask & IN_CREATE)
            {
                printf("I am here in if block\n");
                char message[MAX_MSG_SIZE];
                snprintf(message, MAX_MSG_SIZE,
                         "coredump,path=%s corefile=%s %lld", WATCH_DIR,
                         event->name, get_timestamp());

                printf("%s\n", message);

                if (send_to_mq(message, MESSAGE_QUEUES[0]) == -1)
                {
                    perror("send_to_mq failed");
                }
                printf("After send_to_mq\n");
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}
