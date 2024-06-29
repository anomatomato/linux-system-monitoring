#include "common.h"
#include "disk.h"
#include "mq.h"
#include "net.h"
#include "pid.h"
#include "stat.h"
#include "sys.h"
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
        static struct option long_options[] = {
                {      "proc-stat", 0, 0, 1 },
                {   "proc-net-dev", 0, 0, 2 },
                { "proc-diskstats", 0, 0, 3 },
                {  "proc-pid-stat", 0, 0, 4 },
                { "proc-pid-statm", 0, 0, 5 },
                {    "proc-pid-io", 0, 0, 6 },
                {          "hwmon", 0, 0, 7 },
                {            "sys", 0, 0, 8 },
                {            "pid", 0, 0, 9 },
                {                0, 0, 0, 0 }
        };

        int arg = 0;
        int c = 0;
        int v = 0;
        int h = 0;
        while (1) {
                int rv = 0;
                int option_index = 0;

                rv = getopt_long(argc, argv, "cvh", long_options, &option_index);
                if (rv == -1)
                        break;

                switch (rv) {
                case 99:
                        c = 1;
                        break;
                case 118:
                        v = 1;
                        break;
                case 104:
                        h = 1;
                        break;
                default:
                        if (rv > 9) {
                                printf("Try ./cyclic-file-read-exec -h for help\n");
                                return 1;
                        }
                        arg = rv;
                        break;
                }
        } /*c=99, v=118*/

        if (h) {
                extern char flag[9][MAX_BUFFER];

                printf("Usage: ./cyclic-file-read-exec [FLAG]\n");
                printf("  %s         Send CPU metric\n", flag[0]);
                printf("  %s      Send Networktraffic metric\n", flag[1]);
                printf("  %s    Send Drive metric\n", flag[2]);
                printf("  %s     Send Prozess-Stat metric\n", flag[3]);
                printf("  %s    Send Prozess-Statm metric\n", flag[4]);
                printf("  %s       Send Prozess-IO metric\n", flag[5]);
                printf("  %s             Send Chip-Temperatures\n", flag[6]);
                printf("  %s               Send all System metric\n", flag[7]);
                printf("  %s               Send all Prozess metric\n", flag[8]);
                return 1;
        }

        if (init_mq("/cfr") == -1) /*mq für die bridge erstellen*/
                return 1;

        int timer;
        if ((timer = timerfd_create(CLOCK_REALTIME, 0)) <= 0) { /*timer initialisieren*/
                perror("timerfd_create");
                return 1;
        }
        struct itimerspec timerValue = {
                { 5, 0 },
                { 5, 0 }
        };
        timerfd_settime(timer, 0, &timerValue, NULL); /*timer scharf machen*/

        fd_set rfds; /*den timer beobachten*/
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(timer, &rfds);

        while (1) { /*kontinuierliches sammeln*/
                if (arg == 0 || arg == 1 || arg == 8) {
                        if (stat() == 1) {
                                perror("stat");
                                printf("Failed to read /proc/stat\n");
                        }
                }

                if (arg == 0 || arg == 2 || arg == 8) {
                        if (net() == 1) {
                                perror("net");
                                printf("Failed to read /proc/net/dev\n");
                        }
                }

                if (arg == 0 || arg == 3 || arg == 8) {
                        if (disk() == 1) {
                                perror("disk");
                                printf("Failed to read /proc/diskstats\n");
                        }
                }

                if (arg == 0 || arg == 4 || arg == 9) {
                        if (pid("stat") == 1) {
                                perror("pid");
                                printf("Failed to read /proc/<pid>/stat\n");
                        }
                }

                if (arg == 0 || arg == 5 || arg == 9) {
                        if (pid("statm") == 1) {
                                perror("pid");
                                printf("Failed to read /proc/<pid>/statm\n");
                        }
                }

                if (arg == 0 || arg == 6 || arg == 9) {
                        if (pid("io") == 1) {
                                perror("pid");
                                printf("Failed to read /proc/<pid>/io\n");
                        }
                }

                dequeue(v);

                if (arg == 0 || arg == 7 || arg == 8) {
                        if (sys() == 1) {
                                perror("sys");
                                printf("Failed to read temperatures\n");
                        }
                        dequeue(v);
                }

                select(timer + 1, &rfds, NULL, NULL, NULL);   /*auf timer warten*/
                timerfd_settime(timer, 0, &timerValue, NULL); /*timer neu aufsetzen*/
        }
}
