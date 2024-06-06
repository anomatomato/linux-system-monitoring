#ifndef _INOTIFY_PACCT_H_
#define _INOTIFY_PACCT_H_

// #include <fcntl.h> /* For O_* constants */
// #include <sys/stat.h> /* For mode constants */
#include <linux/limits.h> /* For NAME_MAX */
#include <sys/types.h> /*For type definitions like off_t used in fseek */
#include <sys/inotify.h>  
#include <mqueue.h>
#include "../../utilities/mq.h"
#include "pacct_to_line_protocol.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN                                                          \
    (1024 *                                                                    \
    (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

void monitor_process_accounting();

#endif
