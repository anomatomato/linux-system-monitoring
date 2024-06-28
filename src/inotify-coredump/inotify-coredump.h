#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

#define VERBOSE 1
#define MQ_PATH "/inotify_coredump"
#define WATCH_DIR "/var/lib/systemd/coredump"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */
typedef struct coredump_monitor_t {
        int fd;
        int wd;
        int flags;
} coredump_monitor_t;

int init_inotify(coredump_monitor_t *monitor);
int event_loop(coredump_monitor_t *monitor);
int receive_coredump(char *buffer, coredump_monitor_t *monitor);
int send_coredump(char *buffer, int len, coredump_monitor_t *monitor);
int coredump_to_line_protocol(char *buffer, const char *coredump_name);
int cleanup(coredump_monitor_t *monitor);

/** @brief Whenever a coredump is created, send a message to the message queue
 *
 * @return 0 on success, -1 on failure with errno set appropriately
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int run_inotify_coredump(coredump_monitor_t *monitor);

#endif
