#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

#define VERBOSE 1
#define MQ_PATH "/inotify_coredump"
#define WATCH_DIR "/var/lib/systemd/coredump"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

typedef struct coredump_monitor_t {
        int fd;    /* File descriptor of inotify instance */
        int wd;    /* File descriptor of inotify instance */
        int flags; /* User-defined flags */
} coredump_monitor_t;

/**
 * @brief Initialize the inotify instance and set up a watch on the coredump directory
 *
 * @param monitor Pointer to the coredump_monitor_t structure
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int init_inotify(coredump_monitor_t *monitor);

/**
 * @brief Main event loop to monitor and process inotify events
 *
 * @param monitor Pointer to the coredump_monitor_t structure
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int event_loop(coredump_monitor_t *monitor);

/**
 * @brief Receive coredump events into a buffer
 *
 * @param buffer Pointer to buffer where events will be read
 * @param monitor Pointer to the coredump_monitor_t structure
 *     @retval >= 0  number of bytes read
 *     @retval   -1  on failure with errno set appropriately
 */
int receive_coredump(char *buffer, coredump_monitor_t *monitor);

/**
 * @brief Process and send coredump events from a buffer to the message queue
 *
 * @param buffer Pointer to buffer containing the events
 * @param len Length of the buffer
 * @param monitor Pointer to the coredump_monitor_t structure
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int send_coredump(char *buffer, int len, coredump_monitor_t *monitor);

/**
 * @brief Convert coredump information to line protocol format
 *
 * @param buffer Pointer to buffer where the formatted string will be written
 * @param coredump_name Name of the coredump file
 *     @retval >= 0  length of the formatted string
 *     @retval   -1  on failure
 */
int coredump_to_line_protocol(char *buffer, const char *coredump_name);

/**
 * @brief Clean up resources used for inotify and message queue
 *
 * @param monitor Pointer to the coredump_monitor_t structure
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int cleanup(coredump_monitor_t *monitor);

/**
 * @brief Run the inotify coredump monitoring process
 *
 * This function initializes inotify, enters the main event loop, and cleans up resources on termination.
 *
 * @param monitor Pointer to the coredump_monitor_t structure
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int run_inotify_coredump(coredump_monitor_t *monitor);

#endif
