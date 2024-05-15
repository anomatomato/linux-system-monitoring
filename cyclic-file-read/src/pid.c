#include <stdio.h>
#include <string.h>
#include "pid.h"
#include "common.h"

/*int dir_count() {
        FILE* pipe;
        char buffer[MAX_BUFFER];
        int dir;

        pipe = popen("find /proc -mindepth 1 -maxdepth 1 -type d | wc -l", "r");
        fgets(buffer, MAX_BUFFER, pipe);
        pclose(pipe);

        sscanf(buffer, "%d", &dir);
        return dir;
}*/

int max_pids() {
        FILE* pipe;
        char buffer[MAX_BUFFER];
        int max;

        pipe = popen("cat /proc/sys/kernel/pid_max", "r");
        fgets(buffer, MAX_BUFFER, pipe);
        pclose(pipe);

        sscanf(buffer, "%d", &max);
        return max;
}

int write_pid_message(char* file, char* path, char* id) {
        FILE* pidf;
        if ((pidf = fopen(path, "r")) == NULL)
                return 0;

        /*if (errno == ENOENT)
                return 0;
        else if (errno == EACCES) {
                printf("permission denied");
                return 1;
        }*/

        char line_buffer[MAX_LINE];
        fgets(line_buffer, MAX_LINE, pidf);
        if (line_buffer[strlen(line_buffer) -1] != '\n') {
                printf("File too big.\n");
                return 1;
        }

        char message[MAX_LINE];
        message[0] = '\0';

        char* token;
        token = strtok(line_buffer, " ");

        if (strcmp(file, "stat") == 0) {
                char stat_form[53][MAX_BUFFER] = {"proc-pid-stat,pid=\0", " comm=\0", ",state=\0", ",ppid=\0",
                                                ",pgrp=\0", ",session=\0", ",tty_nr=\0", ",tpgid=\0",
                                                ",flags=\0", ",minflt=\0", ",cminflt=\0", ",majflt=\0",
                                                ",cmajflt=\0", ",utime=\0", ",stime=\0", ",cutime=\0",
                                                ",cstime=\0", ",priority=\0", ",nice=\0", ",num_threads=\0",
                                                ",itrealvalue=\0", ",starttime=\0", ",vsize=\0", ",rss=\0",
                                                ",rsslime=\0", ",startcode=\0", ",endcode=\0", ",startstack=\0",
                                                ",kstkesp=\0", ",kstkeip=\0", ",signal=\0", ",blocked=\0",
                                                ",sigignore=\0", ",sigcatch=\0", ",wchan=\0", ",nswap=\0",
                                                ",cnswap=\0", ",exit_signal=\0", ",processor=\0", ",rt_priority=\0",
                                                ",policy=\0", ",delayacct_blkio_ticks=\0", ",guest_time=\0", ",cguest_time=\0",
                                                ",start_data=\0", ",end_data=\0", ",start_brk=\0", ",arg_start=\0",
                                                ",arg_end=\0", ",env_start=\0", ",env_end=\0", ",exit_code=\0",
                                                " \0"};

                for (int i = 0; i < 51; i++) {
                        strcat(message, stat_form[i]);
                        strcat(message, token);
                        token = strtok(NULL, " ");
                }
                strcat(message, stat_form[51]);
                token[strlen(token) - 1] = '\0';
                strcat(message, token);

                strcat(message, stat_form[52]);

                if (enqueue(message) == 1)
                        return 1;

        } else if (strcmp(file, "statm") == 0) {
                char statm_form[9][MAX_BUFFER] = {"proc-pid-statm,pid=\0", " size=\0", ",resident=\0", ",shared=\0", ",text=\0",
                                                  ",lib=\0", ",data=\0", ",dt=\0", " \0"};

                strcat(message, statm_form[0]);
                strcat(message, id);

                for (int i = 1; i < 7; i++) {
                        strcat(message, statm_form[i]);
                        strcat(message, token);
                        token = strtok(NULL, " ");
                }
                strcat(message, statm_form[7]);
                token[strlen(token) - 1] = '\0';
                strcat(message, token);

                strcat(message, statm_form[8]);

                if (enqueue(message) == 1)
                        return 1;

        } else if (strcmp(file, "io") == 0) {
                char io_form[9][MAX_BUFFER] = {"proc-pid-io,pid=\0", " rchar=\0", ",wchar=\0", ",syscr=\0", ",syscw=\0",
                                               ",read_bytes=\0", ",write_bytes=\0", ",cancelled_write_bytes=\0",
                                               " \0"};

                strcat(message, io_form[0]);
                strcat(message, id);

                for (int i = 1; i < 7; i++) {
                        strcat(message, io_form[i]);
                        strcat(message, token);
                        token = strtok(NULL, " ");
                }
                strcat(message, io_form[7]);
                token[strlen(token) - 1] = '\0';
                strcat(message, token);

                strcat(message, io_form[8]);

                if (enqueue(message) == 1)
                        return 1;
        }

        fclose(pidf);
        return 0;
}

int pid(char* file) {
        int max = max_pids();
        int pnum = 1;
        char path[MAX_BUFFER] = "\0";
        char placeholder[MAX_BUFFER];

        while (pnum <= max) {
                path[0] = '\0';
                strcpy(path, "/proc/\0");

                placeholder[0] = '\0';
                sprintf(placeholder, "%d", pnum);

                strcat(path, placeholder);
                strcat(path, "/");
                strcat(path, file);

                if (write_pid_message(file, path, placeholder) == 1)
                        return 1;

                pnum++;
        }
        return 0;
}

