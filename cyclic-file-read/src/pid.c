#include "pid.h"
#include "common.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int pid_count()
{
    FILE* pipe;
    char buffer[MAX_BUFFER];
    int amount;

    //"find /proc -mindepth 1 -maxdepth 1 -type d | wc -l"
    pipe = popen("ps -e | wc -l",
                 "r"); /*anzahl der momentanen pids durch pipe ermitteln*/
    fgets(buffer, MAX_BUFFER, pipe);
    pclose(pipe);

    sscanf(buffer, "%d", &amount);
    return amount;
}

int max_pids()
{
    FILE* pipe;
    char buffer[MAX_BUFFER];
    int max;

    pipe = popen("cat /proc/sys/kernel/pid_max",
                 "r"); /*anzahl der möglichen pids durch pipe ermitteln*/
    fgets(buffer, MAX_BUFFER, pipe);
    pclose(pipe);

    sscanf(buffer, "%d", &max);
    return max;
}

int read_io_file(char* path, int line)
{ /*io-dateien sind geschützt, daher wird mit pipes gearbeitet*/
    FILE* pipe;
    int val;
    char buffer[MAX_BUFFER];
    char placeholder[MAX_BUFFER];
    sprintf(placeholder, "%d", line);

    char* cmd = "sudo sed -n \0";
    strcat(cmd, placeholder);
    strcat(cmd, "p \0");
    strcat(cmd, path);

    if ((pipe = popen(cmd, "r")) == NULL)
        return -1;
    fgets(buffer, MAX_BUFFER, pipe);
    pclose(pipe);

    char* token;
    token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    sscanf(token, "%d",
           &val); /*konvertierung, da addresse verloren gehen kann*/
    return val;
}

int write_pid_stat(char* path)
{
    FILE* pidf;
    if ((pidf = fopen(path, "r")) == NULL)
    { /*übergebenen pfad öffnen*/
        perror("fopen");
        return 1;
    }

    char line_buffer[MAX_LINE];
    fgets(line_buffer, MAX_LINE, pidf);
    if (line_buffer[strlen(line_buffer) - 1] != '\n')
    {
        printf("File too big.\n"); /*falls zeile zu groß ist*/
        return 1;
    }

    fclose(pidf);

    char message[MAX_LINE];
    message[0] = '\0';

    char* token;
    token = strtok(line_buffer, " "); /*zeile unterteilen*/
    token = strtok(NULL, " ");

    char stat_form[53][MAX_BUFFER] = {"proc-pid-stat,pid=\0",
                                      " comm=\0",
                                      ",state=\0",
                                      ",ppid=\0",
                                      ",pgrp=\0",
                                      ",session=\0",
                                      ",tty_nr=\0",
                                      ",tpgid=\0",
                                      ",flags=\0",
                                      ",minflt=\0",
                                      ",cminflt=\0",
                                      ",majflt=\0",
                                      ",cmajflt=\0",
                                      ",utime=\0",
                                      ",stime=\0",
                                      ",cutime=\0",
                                      ",cstime=\0",
                                      ",priority=\0",
                                      ",nice=\0",
                                      ",num_threads=\0",
                                      ",itrealvalue=\0",
                                      ",starttime=\0",
                                      ",vsize=\0",
                                      ",rss=\0",
                                      ",rsslime=\0",
                                      ",startcode=\0",
                                      ",endcode=\0",
                                      ",startstack=\0",
                                      ",kstkesp=\0",
                                      ",kstkeip=\0",
                                      ",signal=\0",
                                      ",blocked=\0",
                                      ",sigignore=\0",
                                      ",sigcatch=\0",
                                      ",wchan=\0",
                                      ",nswap=\0",
                                      ",cnswap=\0",
                                      ",exit_signal=\0",
                                      ",processor=\0",
                                      ",rt_priority=\0",
                                      ",policy=\0",
                                      ",delayacct_blkio_ticks=\0",
                                      ",guest_time=\0",
                                      ",cguest_time=\0",
                                      ",start_data=\0",
                                      ",end_data=\0",
                                      ",start_brk=\0",
                                      ",arg_start=\0",
                                      ",arg_end=\0",
                                      ",env_start=\0",
                                      ",env_end=\0",
                                      ",exit_code=\0",
                                      " \0"}; /*fürs line protokol*/

    for (int i = 0; i < 51; i++)
    { /*gelesene werte ins format legen*/
        strcat(message, stat_form[i]);
        strcat(message, token);
        token = strtok(NULL, " ");
    }
    strcat(message, stat_form[51]);
    token[strlen(token) - 1] = '\0'; /*'\n' entfernen*/
    strcat(message, token);

    strcat(message, stat_form[52]);

    if (enqueue(message) == 1) /*in die queue anreihen*/
        return 1;

    return 0;
}

int write_pid_statm(char* path, char* id)
{
    FILE* pidf;
    if ((pidf = fopen(path, "r")) == NULL)
    { /*übergebenen pfad öffnen*/
        perror("fopen");
        return 1;
    }

    char line_buffer[MAX_LINE];
    fgets(line_buffer, MAX_LINE, pidf);
    if (line_buffer[strlen(line_buffer) - 1] != '\n')
    {
        printf("File too big.\n"); /*falls zeile zu groß ist*/
        return 1;
    }

    fclose(pidf);

    char message[MAX_LINE];
    message[0] = '\0';

    char* token;
    token = strtok(line_buffer, " "); /*zeile unterteilen*/

    char statm_form[9][MAX_BUFFER] = {"proc-pid-statm,pid=\0",
                                      " size=\0",
                                      ",resident=\0",
                                      ",shared=\0",
                                      ",text=\0",
                                      ",lib=\0",
                                      ",data=\0",
                                      ",dt=\0",
                                      " \0"};

    strcat(message, statm_form[0]);
    strcat(message, id);

    for (int i = 1; i < 7; i++)
    {
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

    return 0;
}

int write_pid_io(char* path, char* id)
{
    char message[MAX_LINE];
    message[0] = '\0';

    char placeholder[MAX_BUFFER];
    int value;

    char io_form[9][MAX_BUFFER] = {"proc-pid-io,pid=\0",
                                   " rchar=\0",
                                   ",wchar=\0",
                                   ",syscr=\0",
                                   ",syscw=\0",
                                   ",read_bytes=\0",
                                   ",write_bytes=\0",
                                   ",cancelled_write_bytes=\0",
                                   " \0"};

    strcat(message, io_form[0]);
    strcat(message, id);

    for (int i = 1; i < 8; i++)
    {
        strcat(message, io_form[i]);
        if (value = read_io_file(path, i) == -1)
            return 1;
        sprintf(placeholder, "%d", value);
        strcat(message, placeholder);
    }
    strcat(message, io_form[8]);

    if (enqueue(message) == 1)
        return 1;

    return 0;
}

int write_pid_message(char* file, char* path, char* id)
{ /*unterteilung der apis*/
    if (strcmp(file, "stat") == 0)
    {
        if (write_pid_stat(path) == 1)
            return 1;
    }
    else if (strcmp(file, "statm") == 0)
    {
        if (write_pid_statm(path, id) == 1)
            return 1;
    }
    else if (strcmp(file, "io") == 0)
    {
        if (write_pid_io(path, id) == 1)
            return 1;
    }

    return 0;
}

int pid(char* file)
{
    int max    = max_pids();  /*maximal mögliche pid anzahl ermitteln*/
    int amount = pid_count(); /*momentane pid anzahl ermitteln*/
    int pnum   = 1;
    int check;
    char path[MAX_BUFFER] = "\0";
    char placeholder[MAX_BUFFER];

    for (int i = 0; i < amount; i++)
    {
        path[0] = '\0';
        strcpy(path, "/proc/\0");

        placeholder[0] = '\0';
        sprintf(placeholder, "%d", pnum);

        strcat(path, placeholder); /*path zu den pid dateien zusammenstellen*/
        strcat(path, "/");
        strcat(path, file);

        if (pnum >
            max) /*zur sicherheit, damit nicht auf ewig pnum inkremiert wird*/
            return 0;

        while ((check = write_pid_message(file, path, placeholder)) != 0)
        { /*fürs protokol erfassen*/
            if (check == 1)
            {           /*wenn eine pid nicht existiert ...*/
                pnum++; /*...die pid versuchen*/

                path[0] = '\0';
                strcpy(path, "/proc/\0");

                placeholder[0] = '\0';
                sprintf(placeholder, "%d", pnum);

                strcat(path, placeholder);
                strcat(path, "/");
                strcat(path, file);

                if (pnum > max) /*zur sicherheit, damit nicht auf ewig pnum
                                   inkremiert wird*/
                    return 0;
            }
        }
        pnum++;
    }
    return 0;
}
