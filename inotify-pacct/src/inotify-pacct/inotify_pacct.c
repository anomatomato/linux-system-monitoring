#include "inotify_pacct.h"

int monitor_process_accounting()
{
    int inotify_fd, watch_fd;
    char buffer[EVENT_BUF_LEN];

    // Initialize inotify
    inotify_fd = inotify_init();
    if (inotify_fd < 0)
    {
        perror("inotify_init");
        return 1;
    }

    // Initialize message queue
    init_mq("/inotify-pacct");

    // Add watch for modifications on the process accounting log file
    watch_fd = inotify_add_watch(inotify_fd, ACCT_FILE, IN_MODIFY);
    if (watch_fd < 0)
    {
        perror("inotify_add_watch");
        close(inotify_fd);
        return 1;
    }

    printf("Monitoring process accounting log: %s\n", ACCT_FILE);

    // Open the process accounting log file
    FILE* acct_file = fopen(ACCT_FILE, "r");
    if (acct_file == NULL)
    {
        perror("fopen acct file failed");
        close(inotify_fd);
        return 1;
    }

    // Seek to the end of the process accounting log file
    fseek(acct_file, 0, SEEK_END);
    long last_position = ftell(acct_file);

    // Start monitoring for modifications
    while (1)
    {
        // Read events from inotify
        int length = read(inotify_fd, buffer, EVENT_BUF_LEN);
        if (length < 0)
        {
            perror("read failed");
            close(inotify_fd);
            return 1;
        }

        int i = 0;
        while (i < length)
        {
            // Process each event
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            if (event->mask & IN_MODIFY)
            {
                // Go to the last known position in the process accounting log file
                fseek(acct_file, last_position, SEEK_SET);

                // Read and process each accounting record
                struct acct_v3 acct_record;
                char payload[MAX_MSG_SIZE];
                while (fread(&acct_record, sizeof(struct acct_v3), 1, acct_file) == 1)
                {
                    double cpu_time = time_comp_to_double(acct_record.ac_utime);
                    double sys_time = time_comp_to_double(acct_record.ac_stime);
                    double avg_mem  = comp_to_double(acct_record.ac_mem);

                    // Check if the record meets the criteria for sending to the message queue
                    if (acct_record.ac_exitcode != 0 || sys_time > 0.00 || cpu_time > 0.00 || avg_mem > 5000.00) {
                        construct_payload(&acct_record, payload, MAX_MSG_SIZE);
                        if (send_to_mq(payload, "/inotify-pacct") == -1)
                        {
                            perror("send_to_mq failed");
                        }
                    }

                    // Remember the position of the file after reading the record
                    last_position = ftell(acct_file);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    // Clean up and return
    inotify_rm_watch(inotify_fd, watch_fd);
    close(inotify_fd);
    fclose(acct_file);
    return 0;
}
