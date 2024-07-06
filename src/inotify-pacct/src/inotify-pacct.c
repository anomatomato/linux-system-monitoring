#include "inotify_pacct.h"

int init_inotify(process_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }

        /* Initialize inotify instance */
        monitor->fd = inotify_init();
        if (monitor->fd == -1) {
                perror("inotify_init failed");
                return -1;
        }

        /* Initialize message queue */
        if (init_mq(MQ_PATH) == -1) {
                perror("init_mq failed");
                close(monitor->fd);
                return -1;
        }

        /* Watch WATCH_DIR for new files */
        monitor->wd = inotify_add_watch(monitor->fd, ACCT_FILE, IN_MODIFY);
        if (monitor->wd < 0) {
                perror("inotify_add_watch failed");
                close(monitor->fd);
                return -1;
        }
        return 0;
}

// Open the process accounting log file
FILE* open_file(process_monitor_t *monitor, char *filename) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return NULL;
        }
        FILE *acct_file = fopen(filename, "r");
        if (acct_file == NULL) {
                perror("fopen acct file failed");
                return NULL;
        }
        return acct_file;
}

int read_event(char *buffer, process_monitor_t *monitor) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }
        /* Read events */
        int len = read(monitor->fd, buffer, EVENT_BUF_LEN);
        if (len < 0) {
                perror("read failed");
                close(monitor->fd);
                return -1;
        }
        return len;
}

int send_process(char *buffer, process_monitor_t *monitor, int length, FILE *acct_file, int last_position) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }

        int i = 0;
        while (i < length) {
                // Process each event
                struct inotify_event *event = (struct inotify_event *) &buffer[i];
                if (event->mask & IN_MODIFY) {
                        // Go to the last known position in the process accounting log file
                        fseek(acct_file, last_position, SEEK_SET);

                        // Read and process each accounting record
                        struct acct_v3 acct_record;
                        char payload[MAX_MSG_SIZE];
                        while (fread(&acct_record, sizeof(struct acct_v3), 1, acct_file) == 1) {

                                construct_payload(&acct_record, payload, MAX_MSG_SIZE);
                                
                                if (monitor->flags == VERBOSE) {
                                        printf("Message: %s\n", payload);
                                }

                                if (send_to_mq(payload, MQ_PATH) == -1) {
                                        perror("send_to_mq failed");
                                        return -1;
                                }
                                
                                // Remember the position of the file after reading the record
                                last_position = ftell(acct_file);
                        }
                }
                i += EVENT_SIZE + event->len;
        }
        return 0;
}

int cleanup(process_monitor_t *monitor, FILE *acct_file) {
        if (!monitor) {
                fprintf(stderr, "monitor not initialized\n");
                return -1;
        }
        (void) inotify_rm_watch(monitor->fd, monitor->wd);
        (void) close(monitor->fd);
        (void) fclose(acct_file);
        (void) remove_mq(MQ_PATH);
        return 0;
}

int monitor_process_accounting(process_monitor_t *monitor) {
  
        printf("Monitoring process accounting log: %s\n", ACCT_FILE);

        if (init_inotify(monitor) == -1) {
                perror("init_inotify failed");
                return -1;
        }  
             
        FILE *acct_file = open_file(monitor, ACCT_FILE);

        if (acct_file == NULL) {
                perror("open_file failed or monitor not initialized\n");
                return -1;
        }

        // Seek to the end of the process accounting log file
        fseek(acct_file, 0, SEEK_END);
        long last_position = ftell(acct_file);

        // Start monitoring for modifications
        while (1) {
                char buffer[EVENT_BUF_LEN];

                // Read events from inotify
                int length = read_event(buffer, monitor);
                if (length == -1) {
                        perror("read_event failed");
                        return -1;
                }

                // Send process accounting data to message queue
                if (send_process(buffer, monitor, length, acct_file, last_position) == -1) {
                        perror("send_process failed");
                        return -1;
                }      
        }
        cleanup(monitor, acct_file);
        return 0;
}
