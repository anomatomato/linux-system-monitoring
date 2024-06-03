#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/acct.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <linux/limits.h> /* For NAME_MAX */
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "mqtt.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN                                                          \
    (1024 *                                                                    \
     (EVENT_SIZE + NAME_MAX + 1)) /* enough for 1024 events in the buffer */

#define ACCT_FILE "/var/log/pacct/acct"
#define POS_FILE "/var/log/pacct/acct_cursor"

const mqtt_config_t config = {
    .host = "tcp://sep-cm0-server.ibr.cs.tu-bs.de:1883/",
    .port = 1883,
    .topic = "linux-monitoring/stats/pacct",
    .client_id = "pacct",
    .qos = 1,
    .timeout = 10000,
};

long long get_timestamp() { return (long long)time(NULL) * 1000000000; }

double comp_to_double(comp_t comp) {
    int exponent = (comp >> 13) & 0x7;      // Extract the 3-bit exponent
    int mantissa = comp & 0x1FFF;           // Extract the 13-bit mantissa
    return mantissa * (1 << exponent);      // Calculate the value
}

void enable_process_accounting(const char *filename) {
    if (acct(filename) == -1) {
        perror("acct");
        exit(EXIT_FAILURE);
    }
    printf("Process accounting enabled and logging to %s\n", filename);
}

void disable_process_accounting() {
    if (acct(NULL) == -1) {
        perror("acct");
        exit(EXIT_FAILURE);
    }
    printf("Process accounting disabled.\n");
}

void save_position(long position) {
    FILE *file = fopen(POS_FILE, "w");
    if (file == NULL) {
        perror("Failed to open position file");
        return;
    }
    fprintf(file, "%ld", position);
    fclose(file);
}

long load_position() {
    FILE *file = fopen(POS_FILE, "r");
    long position = 0;
    if (file != NULL) {
        fscanf(file, "%ld", &position);
        fclose(file);
    }
    return position;
}

char* format_time(time_t start_time) {
    static char formatted_time[64];  // static so that the memory is retained after function return
    if (start_time == 0) {
        snprintf(formatted_time, sizeof(formatted_time), "0");
    } else {
        struct tm *timeinfo = localtime(&start_time);
        strftime(formatted_time, sizeof(formatted_time), "%a-%b-%d-%H:%M:%S-%Y", timeinfo);
    }
    return formatted_time;
}

// void print_acct_record(struct acct_v3 *acct_record) {
//     printf("Process: %s\n", acct_record->ac_comm);
//     printf("PID: %u\n", acct_record->ac_pid);
//     printf("PPID: %u\n", acct_record->ac_ppid);
//     printf("Start Time: %s\n", format_time(acct_record->ac_btime));
//     printf("CPU Time: %.2f seconds\n", comp_to_double(acct_record->ac_utime));
//     printf("System Time: %.2f seconds\n", comp_to_double(acct_record->ac_stime));
//     printf("Average Memory Usage: %.2f KB\n", comp_to_double(acct_record->ac_mem));
//     printf("Exit Code: %u\n", acct_record->ac_exitcode);
//     printf("-------------------------------------------------\n");
// }

void construct_payload(struct acct_v3 *acct_record, char *payload, size_t payload_size) {
    char *start_time_str = format_time(acct_record->ac_btime);
    double cpu_time = comp_to_double(acct_record->ac_utime);
    double sys_time = comp_to_double(acct_record->ac_stime);
    double avg_mem = comp_to_double(acct_record->ac_mem);
    int exit_code = acct_record->ac_exitcode;

    snprintf(
        payload, payload_size,
        "process_accounting,pid=%u,ppid=%u,start_time=%s,cpu_time=%.2f,system_time=%.2f,average_memory_usage=%.2f,exit_code=%d process=\"%s\" %lld",
        acct_record->ac_pid, acct_record->ac_ppid, start_time_str, cpu_time, sys_time, avg_mem, 
        acct_record->ac_exitcode, acct_record->ac_comm, get_timestamp()
    );
}

void monitor_process_accounting() {
    int inotify_fd, watch_fd;
    char buffer[EVENT_BUF_LEN];

    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    watch_fd = inotify_add_watch(inotify_fd, ACCT_FILE, IN_MODIFY);
    if (watch_fd < 0) {
        perror("inotify_add_watch");
        close(inotify_fd);
        exit(EXIT_FAILURE);
    }

    init_mqtt(&config);

    printf("Monitoring process accounting log: %s\n", ACCT_FILE);

    // Load the last position from the file
    long last_position = load_position();
    FILE *acct_file = fopen(ACCT_FILE, "r");
    if (acct_file == NULL) {
        perror("fopen acct file failed");
        close(inotify_fd);
        exit(EXIT_FAILURE);
    }

    // Seek to the last read position
    fseek(acct_file, last_position, SEEK_SET);

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
                char payload[1024];
                while (fread(&acct_record, sizeof(struct acct_v3), 1, acct_file) == 1) {
                    // print_acct_record(&acct_record);
                    construct_payload(&acct_record, payload, 1024);
                    // Print or send the payload to InfluxDB
                    publish_mqtt(&config, payload);
                    // Update the last position
                    last_position = ftell(acct_file);
                    save_position(last_position);
                    
                    printf("%s\n\n", payload);

            	    sleep(1);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(inotify_fd, watch_fd);
    close(inotify_fd);
    close_mqtt(&config);
    fclose(acct_file);
    
}

int main() {
    enable_process_accounting(ACCT_FILE);
    monitor_process_accounting();
    return 0;
}
