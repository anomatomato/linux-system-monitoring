#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_
#define VERBOSE 1

typedef struct coredump_monitor_t {
        int fd;
        int wd;
        int flags;
} coredump_monitor_t;

int init_inotify(coredump_monitor_t *monitor);
int event_loop(coredump_monitor_t *monitor);
int process_events(char *buffer, int len, coredump_monitor_t *monitor);
int coredump_to_line_protocol(char *buffer, const char *coredump_name);
int cleanup(coredump_monitor_t *monitor);

/** @brief Whenever a coredump is created, send a message to the message queue
 *
 * @return 0 on success, -1 on failure with errno set appropriately
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int inotify_coredump(coredump_monitor_t *monitor);

#endif
