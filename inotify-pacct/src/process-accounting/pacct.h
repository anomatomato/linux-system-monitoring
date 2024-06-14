#ifndef _PACCT_H_
#define _PACCT_H_

#include <stdio.h>  // For snprintf, perror, printf
#include <stdlib.h> // For exit, EXIT_FAILURE
#include <unistd.h> // For acct, sysconf

#define ACCT_FILE "/var/log/pacct/acct"

/**
 * Enables process accounting and logs the accounting information to the specified file.
 *
 * @param filename The name of the file to log the accounting information to.
 * @return 0 if process accounting is successfully enabled, 1 otherwise.
 */
int enable_process_accounting(const char* filename);

/**
 * Disables process accounting.
 *
 * This function disables process accounting by calling the `acct` system call with a `NULL` argument.
 * If the `acct` system call fails, an error message is printed and the function returns 1.
 * If the `acct` system call succeeds, a success message is printed and the function returns 0.
 *
 * @return 0 if process accounting is successfully disabled, 1 otherwise.
 */
int disable_process_accounting();

#endif
