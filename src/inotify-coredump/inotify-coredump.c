#include "inotify-coredump.h"
#include "colored_output.h"
#include "line_protocol_parser.h"
#include "mq.h"
#include "signal_handling.h"
#include <errno.h>
#include <fcntl.h>        /* For O_* constants */
#include <linux/limits.h> /* For NAME_MAX */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

#define MQ_PATH "/inotify_coredump"
#define WATCH_DIR "/var/lib/systemd/coredump"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

/* Helper functions */
/*------------------------------------------------------------------------------------------------------*/

int init_inotify(coredump_monitor_t *monitor) {
        /* Initialize inotify instance */
        monitor->fd = inotify_init();
        if (monitor->fd == -1) {
                perror("inotify_init failed");
                return -1;
        }

        /* Initialize message queue */
        if (init_mq(MQ_PATH) == -1) {
                perror("init_mq failed");
                close(monitor->fd);
                return -1;
        }

        /* Watch WATCH_DIR for new files */
        monitor->wd = inotify_add_watch(monitor->fd, WATCH_DIR, IN_CREATE);
        if (monitor->wd < 0) {
                perror("inotify_add_watch failed");
                close(monitor->fd);
                return -1;
        }
        return 0;
}

int coredump_to_line_protocol(char *buffer, const char *coredump_name) {
        return snprintf(buffer,
                        MAX_MSG_SIZE,
                        "coredump,path=%s corefile=\"%s\" %lld",
                        WATCH_DIR,
                        coredump_name,
                        get_timestamp());
}

int process_events(char *buffer, int len, coredump_monitor_t *monitor) {
        for (int i = 0; i < len;) {
                struct inotify_event *event = (struct inotify_event *) &buffer[i];
                if (event->len && event->mask & IN_CREATE) {
                        char message[MAX_MSG_SIZE];
                        coredump_to_line_protocol(message, event->name);

                        if (monitor->flags = VERBOSE)
                                printf("Message:\n%s\n", message);

                        /* Send message to message queue */
                        if (send_to_mq(message, MQ_PATH) == -1) {
                                perror("send_to_mq failed");
                                return -1;
                        }
                }
                i += EVENT_SIZE + event->len;
        }
        return 0;
}

int event_loop(coredump_monitor_t *monitor) {
        char buffer[BUF_LEN];
        /* Event loop */
        while (keep_running) {
                /* Read events */
                int len = read(monitor->fd, buffer, BUF_LEN);
                if (len < 0) {
                        if (errno == EINTR && !keep_running)
                                break;
                        perror("read failed");
                        return -1;
                }
                /* Process events */
                int pe = process_events(buffer, len, monitor);
        }
        return 0;
}

int cleanup(coredump_monitor_t *monitor) {
        int ret1 = inotify_rm_watch(monitor->fd, monitor->wd);
        int ret2 = close(monitor->fd);
        int ret3 = remove_mq(MQ_PATH);
        if (ret1 == -1)
                perror("inotify_rm_watch failed");

        if (ret2 == -1)
                perror("close failed");

        if (ret3 == -1)
                perror("remove_mq failed");

        return (ret1 == -1 || ret2 == -1 || ret3 == -1) ? -1 : 0;
}

int inotify_coredump(coredump_monitor_t *monitor) {
        int fd = monitor->fd;
        int wd = monitor->wd;
        printf("inotify_coredump running...\n");

        if (init_inotify(monitor) == -1)
                return -1;

        printf(COLOR_GREEN STYLE_BOLD "inotify_coredump successfully initialized" RESET_ALL "\n");


        int ret1 = event_loop(monitor);
        int ret2 = cleanup(monitor);
        printf(STYLE_BOLD "%s" RESET_ALL "\n",
               ret2 == 0 ? COLOR_GREEN "cleanup successful" : COLOR_RED "cleanup failed");

        return (ret1 == -1 || ret2 == -1) ? -1 : 0;
}
