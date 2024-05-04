#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/stat.h"
#include "../include/net.h"
#include "../include/disk.h"
#include "../include/common.h"

char* get_btime() {
        FILE* stat;                                                                     /*btime ist in /proc/stat*/

        if ((stat = fopen(STAT_FILE, "r")) == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
        }

        char line_buffer[MAX_LINE];
        char* token;
        int lines = line_count(stat);                                           /*btime an fünft-letzter stelle*/

        for (int i = 0; i < (lines-7); i++) {
                fgets(line_buffer, MAX_LINE, stat);
        }

        fscanf(stat, "%*[^\n]");
        fgets(line_buffer, MAX_LINE, stat);
        fgets(line_buffer, MAX_LINE, stat);
        fgets(line_buffer, MAX_LINE, stat);                     /*mit all den fgets' an die stelle kommen*/

        token = strtok(line_buffer, " ");
        token = strtok(NULL, " ");

        fclose(stat);

        return token;
}

int main(int argc, char* argv[]) {
        char* btime;
        btime = get_btime();                                            /*boot-time ermitteln; jede line braucht das*/

        char time[MAX_BUFFER];
        strcpy(time, btime);                    /*btime kopieren; nach nächstem fopen geht btime verloren*/

        stat(time);
        net(time);
        disk(time);
}
