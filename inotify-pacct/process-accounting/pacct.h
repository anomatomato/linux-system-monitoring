#ifndef _PACCT_H_
#define _PACCT_H_


#include <stdio.h>      // For snprintf, perror, printf
#include <stdlib.h>     // For exit, EXIT_FAILURE
#include <unistd.h>     // For acct, sysconf

#define ACCT_FILE "/var/log/pacct/acct"

void enable_process_accounting(const char *filename);
void disable_process_accounting();

#endif
