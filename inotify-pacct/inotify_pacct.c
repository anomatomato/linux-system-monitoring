#include "../utilities/mq.h"
#include "get_pacct.h"
#include <errno.h>
#include <fcntl.h>        /* For O_* constants */
#include <linux/limits.h> /* For NAME_MAX */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/acct.h>
#include <sys/inotify.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>
#include "inotify_pacct.h"

// server path
// #define WATCH_FILE "/opt/stats-recording/inotify-pacct/src/diff.txt"

#define WATCH_FILE "/var/log/pacct/diff.txt"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN                                                                \
    (1024 *                                                                    \
     (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

/**
 * @file inotify_pacct.c
 * @brief This file contains the implementation of the inotify_pacct function.
 * 
 * The inotify_pacct function sets up an inotify instance, adds a watch for a specified accounting file,
 * and continuously reads events from the inotify file descriptor. When a modification event is detected,
 * the function opens the file, reads its content, and sends the content to message queue.
 * 
 * @return Returns EXIT_SUCCESS if the function executes successfully, otherwise returns EXIT_FAILURE.
 */
int inotify_pacct()
{
    int fd, wd, len;
    char buffer[BUF_LEN];
    printf("inotify_pacct running...\n");
    // Create an inotify instance
    fd = inotify_init();
    init_mq("/inotify_pacct");
    if (fd < 0)
    {
        perror("Failed to initialize inotify");
        return EXIT_FAILURE;
    }

    // Add a watch for the accounting file
    wd = inotify_add_watch(fd, WATCH_FILE, IN_MODIFY);
    if (wd < 0)
    {
        perror("Failed to add inotify watch");
        return EXIT_FAILURE;
    }

    while (1)
    {
        // Read events from the inotify file descriptor
        len = read(fd, buffer, BUF_LEN);
        if (len < 0)
        {
            perror("Failed to read from inotify file descriptor");
            return EXIT_FAILURE;
        }

        // Process each event
        for (int i = 0; i < len;)
        {
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            if (event->mask & IN_MODIFY)
            {
                FILE* diff_file = fopen(WATCH_FILE, "r");
                if (diff_file == NULL)
                {
                    perror("Failed to open diff.txt");
                    return -1;
                }


                // check if the file is empty

                //  moves the file position indicator to the end of the file
                fseek(diff_file, 0, SEEK_END);
                // returns the current value of the file position indicator
                long filesize = ftell(diff_file);
                // moves the file position indicator back to the beginning of
                // the file
                rewind(diff_file);
                
                if (filesize > 0)
                {
                    // The file is not empty, read its content
                    char* message = malloc(filesize + 1);
                    if (message == NULL)
                    {
                        perror("Failed to allocate memory for message");
                        return -1;
                    }

                    fread(message, 1, filesize, diff_file);
                    message[filesize] = '\0'; // Null-terminate the string

                    printf("%s\n", message);

                    if (send_to_mq(message, MESSAGE_QUEUES[1]) == -1)
                    {
                        perror("send_to_mq failed");
                    }

                    free(message);
                }
                else
                {
                    printf("The file is empty.\n");
                }
                fclose(diff_file);
            }
            i += EVENT_SIZE + event->len;
        }
    }

    // Cleanup
    inotify_rm_watch(fd, wd);
    close(fd);

    return EXIT_SUCCESS;
}

// int main()
// {
//     return inotify_pacct();
// }
