#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/disk.h"
#include"../include/common.h"


void write_disk_message(FILE* file, int lines, char* btime) {
        char line_buffer[MAX_LINE];

        char form[15][MAX_BUFFER] = {"proc-diskstats,major=\0", ",minor=\0", ",name=\0",
                             " reads_completed=\0", ",reads_merged=\0", ",sectors_read=\0",
                             ",time_spent_reading=\0", ",writes_completed=\0", ",writes_merged=\0",
                             ",sectors_written=\0", ",time_spent_writing=\0", ",i/os_currently_in_progress=\0",
                             ",time_spent_doing_i/os=\0", ",weighted_time_spent_doing_i/os=\0", " \0"};

        char message[MAX_LINE];
        message[0] = '\0';

        char* token;

        for (int i = 0; i < lines; i++) {
                fgets(line_buffer, MAX_LINE, file);
                token = strtok(line_buffer, " ");

                for (int ii = 0; ii < 14; ii++) {
                        strcat(message, form[ii]);
                        strcat(message, token);
                        token = strtok(NULL, " ");
                }
                strcat(message, form[14]);
                strcat(message, btime);

                printf("%s\n", message);
                message[0] = '\0';
        }
}

void disk(char* btime) {
        FILE* disk;
        int lines;

        if ((disk = fopen(DISK_FILE, "r")) == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
        }

        lines = line_count(disk);

        write_disk_message(disk, lines, btime);

        fclose(disk);
}

