#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/common.h"

int line_count(FILE* file) {
    int lines = 0;

    while (!feof(file)) {
        int ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
    }

    rewind(file);
    return lines;
}

int get_btime() {
    FILE* stat;
    char buffer[128];
    char* token;
    int btime;

    if ((stat = fopen("/proc/stat", "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int lines = line_count(stat);

    for (int i = 0; i < (lines-7); i++) {
        fgets(buffer, 128, stat);
    }

    fscanf(stat, "%*[^\n]");
    fgets(buffer, 128, stat);
    fgets(buffer, 128, stat);

    token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    sscanf(token, "%d", &btime);

    return btime;
}
