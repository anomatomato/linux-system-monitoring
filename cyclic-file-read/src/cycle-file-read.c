#include "../../utilities/mq.h"
#include "common.h"
#include "disk.h"
#include "net.h"
#include "pid.h"
#include "stat.h"
#include "sys.h"
#include "common.h"
#include "../../utilities/mq.h"

int main(int argc, char* argv[]) {
        extern char flag[9][MAX_BUFFER];
        if (argc > 2) {
                printf("Try `./cyclic-file-read-exec -h` for more information");
                return 1;
        }

        if (strcmp(argv[1], "-h") == 0) {
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

        for (int i = 0; i < 9; i++) {
                if (strcmp(argv[1], flag[i]) == 0)
                        break;
                if (i == 8) {
                        printf("Try `./cyclic-file-read-exec -h` for more information");
                        return 1;
                }
        }

        if (init_mq("/cfr") == -1)                                                       /*mq für die bridge erstellen*/
                return 1;

        int timer;
        if ((timer = timerfd_create(CLOCK_REALTIME, 0)) <= 0) {                                 /*timer initialisieren*/
                perror("timerfd_create");
                return 1;
        }
        struct itimerspec timerValue = {{5, 0}, {5, 0}};
        timerfd_settime(timer,  0, &timerValue, NULL);                                           /*timer scharf machen*/

        fd_set rfds;                                                                            /*den timer beobachten*/
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(timer, &rfds);

        while(1) {                                                                          /*kontinuierliches sammeln*/
                if (argc == 1 || strcmp(argv[1], flag[0]) == 0 || strcmp(argv[1], flag[7]) == 0) {
                        if (stat() == 1) {
                                perror("stat");
                                printf("Failed to read /proc/stat\n");
                        }
                }

                if (argc == 1 || strcmp(argv[1], flag[1]) == 0 || strcmp(argv[1], flag[7]) == 0) {
                        if (net() == 1) {
                                perror("net");
                                printf("Failed to read /proc/net/dev\n");
                        }
                }

                if (argc == 1 || strcmp(argv[1], flag[2]) == 0 || strcmp(argv[1], flag[7]) == 0) {
                        if (disk() == 1) {
                                perror("disk");
                                printf("Failed to read /proc/diskstats\n");
                        }
                }

                if (argc == 1 || strcmp(argv[1], flag[3]) == 0 || strcmp(argv[1], flag[8]) == 0) {
                        if (pid("stat") == 1) {
                                perror("pid");
                                printf("Failed to read /proc/<pid>/stat\n");
                        }
                }

                if (argc == 1 || strcmp(argv[1], flag[4]) == 0 || strcmp(argv[1], flag[8]) == 0) {
                        if (pid("statm") == 1) {
                                perror("pid");
                                printf("Failed to read /proc/<pid>/statm\n");
                        }
                }

                if (argc == 1 || strcmp(argv[1], flag[5]) == 0 || strcmp(argv[1], flag[8]) == 0) {
                        if (pid("io") == 1) {
                                perror("pid");
                                printf("Failed to read /proc/<pid>/io\n");
                        }
                }

                dequeue();

                if (argc == 1 || strcmp(argv[1], flag[6]) == 0 || strcmp(argv[1], flag[7]) == 0) {
                        if (sys() == 1) {
                                perror("sys");
                                printf("Failed to read temperatures\n");
                        }
                        dequeue();
                }

        select(timer + 1, &rfds, NULL, NULL, NULL);   /*auf timer warten*/
        timerfd_settime(timer, 0, &timerValue, NULL); /*timer neu aufsetzen*/
    }
}
