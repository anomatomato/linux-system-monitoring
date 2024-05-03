#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/common.h"
#include "../include/stat.h"
#include "../include/net.h"
#include "../include/disk.h"

char* get_btime(FILE* stat, int lines) {
    char buffer[128];
    char* token;

    for (int i = 0; i < (lines-7); i++) {
        fgets(buffer, 128, stat);
    }

    fscanf(stat, "%*[^\n]");
    fgets(buffer, 128, stat);
    fgets(buffer, 128, stat);
    fgets(buffer, 128, stat);

    token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    return token;
}

int main(int argc, char* argv[]) {
    int lines;
    char* btime;
    FILE* stat;

    if ((stat = fopen(FILE_IN, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    lines = line_count(stat);
    btime = get_btime(stat, lines);
    fclose(stat);

    //stat(lines, btime);
    net(btime);
}
