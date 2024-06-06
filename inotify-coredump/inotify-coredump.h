#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

/** @brief Whenever a coredump is created, send a message to the message queue
 *
 * @return 0 on success, -1 on failure with errno set appropriately
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int inotify_coredump();

#endif