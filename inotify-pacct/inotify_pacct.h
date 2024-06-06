#ifndef _INOTIFY_PACCT_H_
#define _INOTIFY_PACCT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For O_* constants */
#include <errno.h>
#include <sys/acct.h>
#include <sys/inotify.h>
#include <sys/stat.h> /* For mode constants */
#include <linux/limits.h> /* For NAME_MAX */
#include <stdint.h> /* For fixed-width integer types*/
#include <string.h>
#include <time.h>
#include <mqueue.h>
#include "../utilities/mq.h"

#define ACCT_FILE "/var/log/pacct/acct"

double comp_to_double(comp_t comp);
double time_comp_to_double(comp_t comp);
void enable_process_accounting(const char *filename);
void disable_process_accounting();
char* format_time(time_t start_time);
const char* get_flag_string(char ac_flag);
void construct_payload(struct acct_v3 *acct_record, char *payload, size_t payload_size);
void monitor_process_accounting();

#endif