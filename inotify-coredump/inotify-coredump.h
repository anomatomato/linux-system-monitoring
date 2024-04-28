#ifndef _INOTIFY_COREDUMP_H_
#define _INOTIFY_COREDUMP_H_

int send_to_mq(const char *message);
int inotify_coredump();

#endif