#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

extern volatile int keep_running;

/**
 * @brief Signal handler function for gracefully shutting down the program.
 *
 * This function is set as the handler for SIGINT, SIGTERM, and other signals.
 * When a signal is received, this function sets keep_running to 0, which indicates the program should
 * stop running.
 *
 * @param signal The signal number of the received signal.
 */
extern void handle_signal(int signal);

/** @brief Whenever a coredump is created, send a message to the message queue
 *
 * @return 0 on success, -1 on failure with errno set appropriately
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int inotify_coredump();

#endif
