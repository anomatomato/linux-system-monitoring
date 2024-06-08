#include "common.h"
#include "../../utilities/mq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Queue *queue_head = NULL;

int line_count(FILE *file) { /*zeilen einer datei zählen*/
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

int enqueue(char *message) {
        Queue *node = (Queue *) malloc(sizeof(Queue));

        if (node) {
                char placeholder[MAX_BUFFER];

                strcpy(node->message, message);
                sprintf(placeholder, "%ld", get_timestamp()); /*timestamp hinzufügen*/
                strcat(node->message, placeholder);
                node->next = NULL;

                if (queue_head == NULL) {
                        queue_head = node;
                        return 0;
                }

                Queue *current = queue_head;
                while (current->next != NULL) { /*queue durchgehen*/
                        current = current->next;
                }
                current->next = node;

        } else { /*falls es kein freier speicher mehr gibt*/
                perror("malloc");
                return 1;
        }
        return 0;
}

void dequeue() {
        if (queue_head == NULL)
                return;

        Queue *current;
        while (queue_head->next != NULL) { /*queue von oben nach unten durchgehen*/
                current = queue_head;
                queue_head = queue_head->next;

                // printf("%s\n", current->message);
                if (send_to_mq(current->message, "/cfr") == -1) /*an die mq senden*/
                        perror("send_to_mq failed");

                free(current); /*alloziiertes freigeben*/
        }
        current = queue_head;

        // printf("%s\n", current->message);
        if (send_to_mq(current->message, "/cfr") == -1)
                perror("send_to_mq failed");

        free(current);
        queue_head = NULL; /*head auf NULL für den nächsten zyklus*/
}

void empty_queue() { /*queue leeren für sys*/
        queue_head = NULL;
}

char flag[9][MAX_BUFFER] = { "--proc-stat",
                             "--proc-net-dev",
                             "--proc-diskstats",
                             "--proc-pid-stat",
                             "--proc-pid-statm",
                             "--proc-pid-io",
                             "--hwmon",
                             "--sys",
                             "--pid" };

char stat_form[13][MAX_BUFFER] = { "proc-stat,core=cpu\0",
                                   " user=\0",
                                   ",nice=\0",
                                   ",system=\0",
                                   ",idle=\0",
                                   ",iowait=\0",
                                   ",irq=\0",
                                   ",softirq=\0",
                                   ",steal=\0",
                                   ",guest=\0",
                                   ",guest_nice=\0",
                                   ",ctext=\0",
                                   " " };

char net_form1[10][MAX_BUFFER] = { "proc-net-dev,interface=\0",
                                   ",direction=receive bytes=\0",
                                   ",packets=\0",
                                   ",errs=\0",
                                   ",drop=\0",
                                   ",fifo=\0",
                                   ",frame=\0",
                                   ",compressed=\0",
                                   ",multicast=\0",
                                   " \0" };

char net_form2[10][MAX_BUFFER] = { "proc-net-dev,interface=\0",
                                   ",direction=transmit bytes=\0",
                                   ",packets=\0",
                                   ",errs=\0",
                                   ",drop=\0",
                                   ",fifo=\0",
                                   ",colls=\0",
                                   ",carrier=\0",
                                   ",compressed=\0",
                                   " \0" };

char disk_form[15][MAX_BUFFER] = { "proc-diskstats,major=\0",
                                   ",minor=\0",
                                   ",name=\0",
                                   " reads_completed=\0",
                                   ",reads_merged=\0",
                                   ",sectors_read=\0",
                                   ",time_spent_reading=\0",
                                   ",writes_completed=\0",
                                   ",writes_merged=\0",
                                   ",sectors_written=\0",
                                   ",time_spent_writing=\0",
                                   ",i/os_currently_in_progress=\0",
                                   ",time_spent_doing_i/os=\0",
                                   ",weighted_time_spent_doing_i/os=\0",
                                   " \0" };

char pstat_form[53][MAX_BUFFER] = { "proc-pid-stat,pid=\0",
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
                                    " \0" };

char pstatm_form[9][MAX_BUFFER] = { "proc-pid-statm,pid=\0",
                                    " size=\0",
                                    ",resident=\0",
                                    ",shared=\0",
                                    ",text=\0",
                                    ",lib=\0",
                                    ",data=\0",
                                    ",dt=\0",
                                    " \0" };

char io_form[9][MAX_BUFFER] = { "proc-pid-io,pid=\0",
                                " rchar=\0",
                                ",wchar=\0",
                                ",syscr=\0",
                                ",syscw=\0",
                                ",read_bytes=\0",
                                ",write_bytes=\0",
                                ",cancelled_write_bytes=\0",
                                " \0" };
