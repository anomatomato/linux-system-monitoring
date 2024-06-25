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

int init_inotify(int *fd, int *wd) {
        /* Initialize inotify instance */
        *fd = inotify_init();
        if (*fd == -1) {
                perror("inotify_init failed");
                return -1;
        }

        /* Initialize message queue */
        if (init_mq(MQ_PATH) == -1) {
                perror("init_mq failed");
                close(*fd);
                return -1;
        }

        /* Watch WATCH_DIR for new files */
        *wd = inotify_add_watch(*fd, WATCH_DIR, IN_CREATE);
        if (*wd < 0) {
                perror("inotify_add_watch failed");
                close(*fd);
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

int process_events(char *buffer, int len, int verbose) {
        for (int i = 0; i < len;) {
                struct inotify_event *event = (struct inotify_event *) &buffer[i];
                if (event->len && event->mask & IN_CREATE) {
                        char message[MAX_MSG_SIZE];
                        coredump_to_line_protocol(message, event->name);

                        if (verbose)
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

int event_loop(int fd, int test, int verbose) {
        char buffer[BUF_LEN];
        /* Event loop */
        while (keep_running) {
                /* Read events */
                int len = read(fd, buffer, BUF_LEN);
                if (len < 0) {
                        if (errno == EINTR && !keep_running)
                                break;
                        perror("read failed");
                        return -1;
                }
                /* Process events */
                int pe = process_events(buffer, len, verbose);
        }
        return 0;
}

int cleanup(int fd, int wd) {
        int ret1 = inotify_rm_watch(fd, wd);
        int ret2 = close(fd);
        int ret3 = remove_mq(MQ_PATH);
        if (ret1 == -1)
                perror("inotify_rm_watch failed");

        if (ret2 == -1)
                perror("close failed");

        if (ret3 == -1)
                perror("remove_mq failed");

        return (ret1 == -1 || ret2 == -1 || ret3 == -1) ? -1 : 0;
}

int inotify_coredump(int test, int verbose) {
        int fd = -1, wd = -1;
        printf("inotify_coredump running...\n");

        if (init_inotify(&fd, &wd) == -1)
                return -1;

        if (test)
                printf(COLOR_GREEN STYLE_BOLD "inotify_coredump successfully initialized" RESET_ALL "\n");


        int ret1 = event_loop(fd, test, verbose);
        int ret2 = cleanup(fd, wd);
        if (test)
                printf(STYLE_BOLD "%s" RESET_ALL "\n",
                       ret2 == 0 ? COLOR_GREEN "cleanup successful" : COLOR_RED "cleanup failed");

        return (ret1 == -1 || ret2 == -1) ? -1 : 0;
}
