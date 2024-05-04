#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/net.h"
#include"../include/common.h"


void write_net_message(FILE* file, int lines, char* btime) {
        char line_buffer[MAX_LINE];
        fgets(line_buffer, MAX_LINE, file);
        fgets(line_buffer, MAX_LINE, file);                     /*erste zwei zeilen uninteresant*/

        char form1[10][MAX_BUFFER] = {"proc-net-dev,interface=\0", ",direction=receive bytes=\0", ",packets=\0",
                          ",errs=\0", ",drop=\0", ",fifo=\0", ",frame=\0", ",compressed=\0",
                          ",multicast=\0", " \0"};                              /*einmal für recieving*/
        char form2[10][MAX_BUFFER] = {"proc-net-dev,interface=\0", ",direction=transmit bytes=\0", ",packets=\0",
                          ",errs=\0", ",drop=\0", ",fifo=\0", ",colls=\0", ",carrier=\0",
                          ",compressed=\0", " \0"};                             /*einmal für transfer*/

        char message[MAX_LINE];
        message[0] = '\0';

        char name[MAX_BUFFER];
        name[0] = '\0';                                                                 /*name wird zweimal benötigt*/

        char* token;

        for (int l = 0; l < lines - 2; l++) {
                strcat(message, form1[0]);

                fgets(line_buffer, MAX_LINE, file);
                token = strtok(line_buffer, " ");
                token[strlen(token) -1] = '\0';
                strcat(message, token);

                strcpy(name, token);

                for (int i = 1; i < 9; i++) {                                   /*gelesene werte ins format legen*/
                        strcat(message, form1[i]);
                        token = strtok(NULL, " ");
                        strcat(message, token);
                }
                strcat(message, form1[9]);
                strcat(message, btime);

                printf("%s\n", message);
                message[0] = '\0';                                                      /*message wiederverwenden*/

                strcat(message, form1[0]);
                strcat(message, name);
                name[0] = '\0';                                                                 /*name wiederverwenden*/

                for (int i = 1; i < 9; i++) {                                   /*gelesene werte für gegenrichtung*/
                        strcat(message, form2[i]);
                        token = strtok(NULL, " ");
                        strcat(message, token);
                }
                strcat(message, form2[9]);
                strcat(message, btime);

                printf("%s\n", message);
                message[0] = '\0';
        }
}

void net(char* btime) {
        FILE* net;
        int lines;

        if ((net = fopen(NET_FILE, "r")) == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
        }

        lines = line_count(net);

        write_net_message(net, lines, btime);

        fclose(net);
}
