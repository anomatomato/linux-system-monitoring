#include "inotify_pacct.h"

void monitor_process_accounting() {
    int inotify_fd, watch_fd;
    char buffer[EVENT_BUF_LEN];

    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    init_mq("/inotify-pacct");

    watch_fd = inotify_add_watch(inotify_fd, ACCT_FILE, IN_MODIFY);
    if (watch_fd < 0) {
        perror("inotify_add_watch");
        close(inotify_fd);
        exit(EXIT_FAILURE);
    }

    printf("Monitoring process accounting log: %s\n", ACCT_FILE);

    FILE *acct_file = fopen(ACCT_FILE, "r");
    if (acct_file == NULL) {
        perror("fopen acct file failed");
        close(inotify_fd);
        exit(EXIT_FAILURE);
    }

    // seek to end of pacct file
    fseek(acct_file, 0, SEEK_END);

    while (1) {
        int length = read(inotify_fd, buffer, EVENT_BUF_LEN);
        if (length < 0) {
            perror("read failed");
            close(inotify_fd);
            exit(EXIT_FAILURE);
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->mask & IN_MODIFY) {
                struct acct_v3 acct_record;
                char payload[MAX_MSG_SIZE];
                while (fread(&acct_record, sizeof(struct acct_v3), 1, acct_file) == 1) {
                    double cpu_time = time_comp_to_double(acct_record.ac_utime);
                    double sys_time = time_comp_to_double(acct_record.ac_stime);
                    double avg_mem = comp_to_double(acct_record.ac_mem);
                
                    // if (acct_record.ac_exitcode != 0 || sys_time > 0.00 || cpu_time > 0.00 || avg_mem > 5000.00) {
                        construct_payload(&acct_record, payload, MAX_MSG_SIZE);
                        if (send_to_mq(payload, "/inotify-pacct") == -1)
                        {
                            perror("send_to_mq failed");
                        }
                    // }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(inotify_fd, watch_fd);
    close(inotify_fd);
    fclose(acct_file);  
}
