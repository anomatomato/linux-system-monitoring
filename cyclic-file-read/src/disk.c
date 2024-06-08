#include "disk.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

int write_disk_message(FILE* file, int lines)
{
    char line_buffer[MAX_LINE];

        extern char disk_form[15][MAX_BUFFER];
                                                                                                  /*fürs line protokol*/
        char message[MAX_LINE];
        message[0] = '\0';

    char* token;

    for (int i = 0; i < lines; i++)
    {
        fgets(line_buffer, MAX_LINE, file);
        if (line_buffer[strlen(line_buffer) - 1] != '\n')
        {
            printf("File too big.\n"); /*falls zeile zu groß ist*/
            return 1;
        }

        token = strtok(line_buffer, " "); /*zeile unterteilen*/

                for (int ii = 0; ii < 14; ii++) {                                    /*gelesene werte ins format legen*/
                        strcat(message, disk_form[ii]);
                        strcat(message, token);
                        token = strtok(NULL, " ");
                }
                if (line_buffer[strlen(line_buffer) -1] == '\n')
                        line_buffer[strlen(line_buffer) -1] = '\0';

                strcat(message, disk_form[14]);

        if (enqueue(message) == 1) /*in die queue anreihen*/
            return 1;

        message[0] = '\0'; /*message wiederverwenden*/
    }
    return 0;
}

int disk()
{
    FILE* diskf;
    int lines;

    if ((diskf = fopen(DISK_FILE, "r")) == NULL)
    { /*datei der werte öffnen*/
        perror("fopen");
        return 1;
    }

    lines = line_count(diskf); /*für die anzahl der partitions*/

    if (write_disk_message(diskf, lines) == 1) /*fürs line protokol anreihen*/
        return 1;

    fclose(diskf); /*datei schließen*/
    return 0;
}
