#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

int init_inotify(int *fd, int *wd);
int process_events(int fd, int test, int verbose);
int cleanup(int fd, int wd);

/** @brief Whenever a coredump is created, send a message to the message queue
 *
 * @return 0 on success, -1 on failure with errno set appropriately
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int inotify_coredump(int test, int verbose);

#endif
