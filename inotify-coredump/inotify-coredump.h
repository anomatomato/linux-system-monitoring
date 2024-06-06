#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

/** @brief Whenever a coredump is created, send a message to the message queue
 *
 * @return Returns EXIT_SUCCESS if the function executes successfully, otherwise
 * returns EXIT_FAILURE.
 */
int inotify_coredump();

#endif