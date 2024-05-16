#include "inotify_pacct.h"
#include "../utilities/mq.h"
#include <errno.h>
#include <fcntl.h>        /* For O_* constants */
#include <linux/limits.h> /* For NAME_MAX */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/acct.h>
#include <sys/inotify.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

#define BINARY "/var/log/pacct/binary"
#define PACCT "/var/log/pacct/pacct"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN                                                                \
    (1024 *                                                                    \
     (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

int main()
{
    int fd, wd, len;
    char buffer[BUF_LEN];
    printf("inotify_pacct running...\n");
    // Create an inotify instance
    fd = inotify_init();
    // init_mq("/inotify_pacct");
    if (fd < 0)
    {
        perror("Failed to initialize inotify");
        return EXIT_FAILURE;
    }

    // Add a watch for the accounting file
    wd = inotify_add_watch(fd, PACCT, IN_MODIFY);
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
                FILE* fp = fopen(PACCT, "r");
                if (fp == NULL)
                {
                    perror("Failed to open file");
                    return 1;
                }

                struct acct_v3 account;

                char message[MAX_MSG_SIZE];

                while(fread(&account, sizeof(struct acct_v3), 1, fp) == 1)
                {
                    acc
                }

                snprintf(message, 1024,
                         "pacct,raspi=pi-85cd "
                         "ac_flag=%c,ac_version=%c,ac_tty=%u,ac_exitcode=%u,ac_"
                         "uid=%u,"
                         "ac_gid=%u,ac_pid=%u,ac_ppid=%u,ac_btime=%f,ac_etime=%d,ac_"
                         "utime=%u,ac_stime=%u,ac_mem=%u,ac_io=%u,ac_rw=%u,ac_"
                         "minflt=%u,ac_majflt=%u,ac_swaps=%u,ac_comm=\"%s\"",
                         account.ac_flag, account.ac_version, account.ac_tty,
                         account.ac_exitcode, account.ac_uid, account.ac_gid,
                         account.ac_pid, account.ac_ppid, account.ac_btime,
                         account.ac_etime, account.ac_utime, account.ac_stime,
                         account.ac_mem, account.ac_io, account.ac_rw,
                         account.ac_minflt, account.ac_majflt, account.ac_swaps,
                         account.ac_comm);
                printf("Message: %s\n", message);
                // if (send_to_mq(message, MESSAGE_QUEUES[1]) == -1)
                // {
                //     perror("send_to_mq failed");
                // }
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
