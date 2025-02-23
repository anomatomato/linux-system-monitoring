#ifndef _INOTIFY_PACCT_H_
#define _INOTIFY_PACCT_H_

#include "pacct_to_line_protocol.h"
#include <linux/limits.h> /* For NAME_MAX */
#include <mqueue.h>
#include <sys/inotify.h>
#include <sys/types.h> /*For type definitions like off_t used in fseek */

#define VERBOSE 1
#define MQ_PATH "/inotify_pacct"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

typedef struct process_monitor_t {
        int fd;
        int wd;
        int flags;
} process_monitor_t;


int init_inotify(process_monitor_t *monitor);
FILE* open_file(process_monitor_t *monitor, char *filename);
int read_event(char *buffer, process_monitor_t *monitor);
int send_process(char *buffer, process_monitor_t *monitor, int length, FILE *acct_file, int last_position);
/**
 * Monitors a process accounting log file for modifications and sends relevant data to a message queue.
 *
 * @return 0 if the program executed successfully, otherwise a non-zero value.
 */
int monitor_process_accounting(process_monitor_t *monitor);
int cleanup(process_monitor_t *monitor, FILE *acct_file);

#endif
