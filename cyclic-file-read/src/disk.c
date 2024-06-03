#include <stdio.h>
#include <string.h>
#include "disk.h"
#include "common.h"

int write_disk_message(FILE* file, int lines) {
        char line_buffer[MAX_LINE];

        char form[15][MAX_BUFFER] = {"proc-diskstats,major=\0", ",minor=\0", ",name=\0",
                             " reads_completed=\0", ",reads_merged=\0", ",sectors_read=\0",
                             ",time_spent_reading=\0", ",writes_completed=\0", ",writes_merged=\0",
                             ",sectors_written=\0", ",time_spent_writing=\0", ",i/os_currently_in_progress=\0",
                             ",time_spent_doing_i/os=\0", ",weighted_time_spent_doing_i/os=\0", " \0"};
                                                                                                  /*fürs line protokol*/
        char message[MAX_LINE];
        message[0] = '\0';

        char* token;

        for (int i = 0; i < lines; i++) {
                fgets(line_buffer, MAX_LINE, file);
                if (line_buffer[strlen(line_buffer) -1] != '\n') {
                        printf("File too big.\n");                                           /*falls zeile zu groß ist*/
                        return 1;
                }

                token = strtok(line_buffer, " ");                                                  /*zeile unterteilen*/

                for (int ii = 0; ii < 14; ii++) {                                    /*gelesene werte ins format legen*/
                        strcat(message, form[ii]);
                        strcat(message, token);
                        token = strtok(NULL, " ");
                }
                if (line_buffer[strlen(line_buffer) -1] == '\n')                                      /*'\n' entfernen*/
                        line_buffer[strlen(line_buffer) -1] = '\0';

                strcat(message, form[14]);

                if (enqueue(message) == 1)                                                     /*in die queue anreihen*/
                        return 1;

                message[0] = '\0';                                                           /*message wiederverwenden*/
        }
        return 0;
}

int disk() {
        FILE* diskf;
        int lines;

        if ((diskf = fopen(DISK_FILE, "r")) == NULL) {                                        /*datei der werte öffnen*/
                perror("fopen");
                return 1;
        }

        lines = line_count(diskf);                                                     /*für die anzahl der partitions*/

        if (write_disk_message(diskf, lines) == 1)                                       /*fürs line protokol anreihen*/
                return 1;

        fclose(diskf);                                                                               /*datei schließen*/
        return 0;
}
