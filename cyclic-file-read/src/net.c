#include "net.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

int write_net_message(FILE* file, int lines)
{
    char line_buffer[MAX_LINE];
    fgets(line_buffer, MAX_LINE, file);
    fgets(line_buffer, MAX_LINE, file); /*erste zwei zeilen uninteresant*/

    char form1[10][MAX_BUFFER] = {"proc-net-dev,interface=\0",
                                  ",direction=receive bytes=\0",
                                  ",packets=\0",
                                  ",errs=\0",
                                  ",drop=\0",
                                  ",fifo=\0",
                                  ",frame=\0",
                                  ",compressed=\0",
                                  ",multicast=\0",
                                  " \0"}; /*einmal für recieving*/
    char form2[10][MAX_BUFFER] = {"proc-net-dev,interface=\0",
                                  ",direction=transmit bytes=\0",
                                  ",packets=\0",
                                  ",errs=\0",
                                  ",drop=\0",
                                  ",fifo=\0",
                                  ",colls=\0",
                                  ",carrier=\0",
                                  ",compressed=\0",
                                  " \0"}; /*einmal für transfer*/

    char message[MAX_LINE];
    message[0] = '\0';

    char name[MAX_BUFFER];
    name[0] = '\0'; /*name wird zweimal benötigt*/

    char* token;

    for (int l = 0; l < lines - 2; l++)
    { /*lines-2 ist anzahl der interfaces*/
        strcat(message, form1[0]);

        fgets(line_buffer, MAX_LINE, file);
        if (line_buffer[strlen(line_buffer) - 1] != '\n')
        {
            printf("File too big.\n"); /*falls zeile zu groß ist*/
            return 1;
        }

        token = strtok(line_buffer, " "); /*zeile unterteilen*/
        token[strlen(token) - 1] = '\0';  /*':' entfernen*/
        strcat(message, token);

        strcpy(name, token);

        for (int i = 1; i < 9; i++)
        { /*gelesene werte ins format legen*/
            strcat(message, form1[i]);
            token = strtok(NULL, " ");
            strcat(message, token);
        }
        strcat(message, form1[9]);

        if (enqueue(message) == 1) /*in die queue anreihen*/
            return 1;

        message[0] = '\0'; /*message wiederverwenden*/

        strcat(message, form1[0]);
        strcat(message, name);
        name[0] = '\0'; /*name wiederverwenden*/

        for (int i = 1; i < 8; i++)
        { /*gelesene werte für gegenrichtung*/
            strcat(message, form2[i]);
            token = strtok(NULL, " ");
            strcat(message, token);
        }
        strcat(message, form2[8]);
        token                    = strtok(NULL, " ");
        token[strlen(token) - 1] = '\0'; /*'\n' entfernen*/
        strcat(message, token);

        strcat(message, form2[9]);

        if (enqueue(message) == 1)
            return 1;

        message[0] = '\0';
    }
    return 0;
}

int net()
{
    FILE* netf;
    int lines;

    if ((netf = fopen(NET_FILE, "r")) == NULL)
    { /*datei der werte öffnen*/
        perror("fopen");
        return 1;
    }

    lines = line_count(netf); /*für die anzahl der interfaces*/

    if (write_net_message(netf, lines) == 1) /*fürs line protokol anreihen*/
        return 1;

    fclose(netf); /*datei schließen*/
    return 0;
}
