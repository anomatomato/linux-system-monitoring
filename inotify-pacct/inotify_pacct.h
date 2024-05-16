#ifndef _GET_PACCT_H_
#define _GET_PACCT_H_

/**
 * Converts binary accounting data to a human-readable text file.
 * The function uses the `dump-acct` command to convert the binary accounting
 * data stored in `binary_acct` to a human-readable format and writes it to the
 * specified text file located at
 * `/var/log/pacct`.
 *
 * @return Returns `EXIT_SUCCESS` if the conversion and writing process is
 * successful, otherwise returns `EXIT_FAILURE`.
 */
int convert_binary_to_text();

/**
 * Enables process accounting, saves records to a specified file,
 *
 * @return EXIT_SUCCESS if process accounting is enabled and disabled
 * successfully, EXIT_FAILURE otherwise.
 */
int enable_process_accounting();

/**
 * The inotify_pacct function sets up an inotify instance, adds a watch for a
 * specified accounting file, and reads events from the inotify
 * file descriptor. When a modification event is detected, the metrics from acct instance are gotten
 * and sent to a message queue.
 * 
 * @return Returns EXIT_SUCCESS if the function executes successfully, otherwise
 * returns EXIT_FAILURE.
 */
int inotify_pacct();

#endif