#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/common.h"
#include"../include/net.h"

void write_message(FILE* file, int lines, char* btime) {
    char buffer[256];
    fgets(buffer, 256, file);
    fgets(buffer, 256, file);

    char form1[10][32] = {"proc-net-dev,interface=\0", ",direction=receive bytes=\0", ",packets=\0", ",errs=\0", ",drop=\0", ",fifo=\0", ",frame=\0", ",compressed=\0", ",multicast=\0", " \0"};
    char form2[10][32] = {"proc-net-dev,interface=\0", ",direction=transmit bytes=\0", ",packets=\0", ",errs=\0", ",drop=\0", ",fifo=\0", ",colls=\0", ",carrier=\0", ",compressed=\0", " \0"};

    char message[256];
    char name[32];
    char* token;

    for (int l = 0; l < lines - 2; l++) {

        message[0] = '\0';
        name[0] = '\0';
        strcat(message, form1[0]);

        fgets(buffer, 256, file);
        token = strtok(buffer, " ");

        token[strlen(token) -1] = '\0';
        strcat(message, token);
        strcpy(name, token);

        for (int i = 1; i < 9; i++) {
            strcat(message, form1[i]);
            token = strtok(NULL, " ");
            strcat(message, token);
        }
        strcat(message, form1[9]);

        strcat(message, btime);
        printf("%s\n", message);

        message[0] = '\0';
        strcat(message, form1[0]);
        strcat(message, name);

        for (int i = 1; i < 9; i++) {
            strcat(message, form2[i]);
            token = strtok(NULL, " ");
            strcat(message, token);
        }
        strcat(message, form2[9]);

        strcat(message, btime);
        printf("%s\n", message);
    }
}

void net(char* btime) {
    FILE* net;
    int lines;
    char time[16];
    strcpy(time, btime);

    if ((net = fopen("/proc/net/dev", "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    lines = line_count(net);

    write_message(net, lines, time);

    fclose(net);
}
