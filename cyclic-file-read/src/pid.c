#include "pid.h"
#include "common.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int pid_count(char *cmd) { /*anzahl dr prozesse ermitteln*/
        FILE *pipe;
        char buffer[MAX_BUFFER];
        int amount;

        pipe = popen(cmd, "r");
        fgets(buffer, MAX_BUFFER, pipe);
        pclose(pipe);

        sscanf(buffer, "%d", &amount);
        return amount;
}

int dirctory_check(char *path) { /*überprüfen ob prozess existiert*/
        DIR *dir = opendir(path);
        if (dir) {
                closedir(dir);
                return 0;
        }
        return 1;
}

int write_pid_stat(char *path) {
        FILE *pidf;
        if ((pidf = fopen(path, "r")) == NULL) { /*übergebenen pfad öffnen*/
                perror("fopen");
                return 1;
        }

        char line_buffer[MAX_LINE];
        fgets(line_buffer, MAX_LINE, pidf);
        if (line_buffer[strlen(line_buffer) - 1] != '\n') {
                printf("File too big.\n"); /*falls zeile zu groß ist*/
                return 1;
        }

        fclose(pidf);

        char message[MAX_LINE];
        message[0] = '\0';

        char *token;
        token = strtok(line_buffer, " "); /*zeile unterteilen*/

        extern char pstat_form[53][MAX_BUFFER]; /*fürs line protokol*/

        for (int i = 0; i < 51; i++) { /*gelesene werte ins format legen*/
                strcat(message, pstat_form[i]);
                strcat(message, token);
                token = strtok(NULL, " ");
        }
        strcat(message, pstat_form[51]);
        token[strlen(token) - 1] = '\0'; /*'\n' entfernen*/
        strcat(message, token);

        strcat(message, pstat_form[52]);

        if (enqueue(message) == 1) /*in die queue anreihen*/
                return 1;

        return 0;
}

int write_pid_statm(char *path, char *id) {
        FILE *pidf;
        if ((pidf = fopen(path, "r")) == NULL) { /*übergebenen pfad öffnen*/
                perror("fopen");
                return 1;
        }

        char line_buffer[MAX_LINE];
        fgets(line_buffer, MAX_LINE, pidf);
        if (line_buffer[strlen(line_buffer) - 1] != '\n') {
                printf("File too big.\n"); /*falls zeile zu groß ist*/
                return 1;
        }

        fclose(pidf);

        char message[MAX_LINE];
        message[0] = '\0';

        char *token;
        token = strtok(line_buffer, " "); /*zeile unterteilen*/

        extern char pstatm_form[9][MAX_BUFFER];

        strcat(message, pstatm_form[0]);
        strcat(message, id);

        for (int i = 1; i < 7; i++) {
                strcat(message, pstatm_form[i]);
                strcat(message, token);
                token = strtok(NULL, " ");
        }
        strcat(message, pstatm_form[7]);
        token[strlen(token) - 1] = '\0';
        strcat(message, token);

        strcat(message, pstatm_form[8]);

        if (enqueue(message) == 1)
                return 1;

        return 0;
}

int write_pid_io(char *path, char *id) {
        char message[MAX_LINE];
        message[0] = '\0';

        char line_buffer[MAX_LINE];

        char *token;

        char cmd[MAX_BUFFER];
        sprintf(cmd, "sudo cat %s", path);

        FILE *pipe;
        pipe = popen(cmd, "r"); /*io-datei schreibgeschützt, daher wird mit pipes gearbeitet*/

        extern char io_form[9][MAX_BUFFER];

        strcat(message, io_form[0]);
        strcat(message, id);

        for (int i = 1; i < 8; i++) {
                strcat(message, io_form[i]);

                fgets(line_buffer, MAX_BUFFER, pipe);
                token = strtok(line_buffer, " ");
                token = strtok(NULL, " ");
                token[strlen(token) - 1] = '\0';

                strcat(message, token);
        }
        strcat(message, io_form[8]);

        pclose(pipe);

        if (enqueue(message) == 1)
                return 1;

        return 0;
}

int write_pid_message(char *file, char *path, char *id) { /*unterteilung der apis*/
        if (strcmp(file, "stat") == 0) {
                if (write_pid_stat(path) == 1)
                        return 1;
        } else if (strcmp(file, "statm") == 0) {
                if (write_pid_statm(path, id) == 1)
                        return 1;
        } else if (strcmp(file, "io") == 0) {
                if (write_pid_io(path, id) == 1)
                        return 1;
        }

        return 0;
}

int pid(char *file) {
        int max = pid_count("cat /proc/sys/kernel/pid_max"); /*maximal mögliche pid anzahl ermitteln*/
        int amount = pid_count("ps -e | wc -l");             /*momentane pid anzahl ermitteln*/
        int pnum = 1;
        char path[MAX_BUFFER] = "\0";
        char placeholder[MAX_BUFFER];

        for (int i = 0; i < amount; i++) {
                sprintf(path, "/proc/%d", pnum);

                while (dirctory_check(path) == 1) {
                        pnum++;
                        sprintf(path, "/proc/%d", pnum);
                }

                if (pnum > max) /*zur sicherheit, damit nicht auf ewig pnum inkremiert wird*/
                        return 0;

                sprintf(path, "/proc/%d/%s", pnum, file);
                sprintf(placeholder, "%d", pnum);
                if (write_pid_message(file, path, placeholder) == 1)
                        return 1;

                pnum++;
        }
        return 0;
}
