#include "inotify-coredump.h"
#include "colored_output.h"
#include "line_protocol_escaper.h"
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

int init_inotify(coredump_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }

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
        char tag_path[MAX_MSG_SIZE];
        char field_coredump_name[MAX_MSG_SIZE];

        int rt1 = escape_tag_value(WATCH_DIR, tag_path, MAX_MSG_SIZE);
        int rt2 = escape_field_value(coredump_name, field_coredump_name, MAX_MSG_SIZE);
        if (rt2 == -1 || rt2 == -1)
                return -1;

        return snprintf(buffer,
                        MAX_MSG_SIZE,
                        "coredump,path=%s corefile=\"%s\" %lld",
                        tag_path,
                        field_coredump_name,
                        get_timestamp());
}

int send_coredump(char *buffer, int len, coredump_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }

        int i = 0;
        while (i < len) {
                struct inotify_event *event = (struct inotify_event *) &buffer[i];
                if (event->len && event->mask & IN_CREATE) {
                        char message[MAX_MSG_SIZE];
                        int ret = coredump_to_line_protocol(message, event->name);
                        if (ret < 0) {
                                perror("coredump_to_line_protocol failed");
                                return -1;
                        }

                        if (monitor->flags == VERBOSE)
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

int receive_coredump(char *buffer, coredump_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }
        /* Read events */
        int len = read(monitor->fd, buffer, BUF_LEN);
        if (len < 0) {
                if (errno == EINTR && !keep_running)
                        return 0;
                perror("read failed");
                return -1;
        }
        printf("%ld", sizeof(struct inotify_event));
        return len;
}

int event_loop(coredump_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }

        char buffer[BUF_LEN];
        /* Event loop */
        while (keep_running) {
                /* Read events */
                int len = receive_coredump(buffer, monitor);
                if (len == -1)
                        return -1;

                /* Process events */
                int pe = send_coredump(buffer, len, monitor);
                if (pe == -1)
                        return -1;
        }
        return 0;
}

int cleanup(coredump_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }
        (void) inotify_rm_watch(monitor->fd, monitor->wd);
        (void) close(monitor->fd);
        (void) remove_mq(MQ_PATH);
        return 0;
}

int run_inotify_coredump(coredump_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }

        printf("inotify_coredump running...\n");

        if (init_inotify(monitor) == -1)
                return -1;

        printf(COLOR_GREEN STYLE_BOLD "inotify_coredump successfully initialized" RESET_ALL "\n");

        int ret = event_loop(monitor);
        cleanup(monitor);

        return ret;
}
