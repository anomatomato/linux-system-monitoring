#include "../../utilities/mq.h"
#include "common.h"
#include "disk.h"
#include "net.h"
#include "pid.h"
#include "stat.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <time.h>

int main(int argc, char* argv[])
{
    if (init_mq("/cfr") == -1) /*mq für die bridge erstellen*/
        return 1;

    int timer;
    if ((timer = timerfd_create(CLOCK_REALTIME, 0)) <= 0)
    { /*timer initialisieren*/
        perror("timerfd_create");
        return 1;
    }
    struct itimerspec timerValue = {{5, 0}, {5, 0}};
    timerfd_settime(timer, 0, &timerValue, NULL); /*timer scharf machen*/

    fd_set rfds; /*den timer beobachten*/
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    FD_SET(timer, &rfds);

    while (1)
    { /*kontinuierliches sammeln*/
        if (argc == 1 || strcmp(argv[1], "--proc-stat") == 0)
        {
            if (stat() == 1)
            {
                perror("stat");
                printf("Failed to read /proc/stat\n");
            }
        }

        if (argc == 1 || strcmp(argv[1], "--proc-net-dev") == 0)
        {
            if (net() == 1)
            {
                perror("net");
                printf("Failed to read /proc/net/dev\n");
            }
        }

        if (argc == 1 || strcmp(argv[1], "--proc-diskstats") == 0)
        {
            if (disk() == 1)
            {
                perror("disk");
                printf("Failed to read /proc/diskstats\n");
            }
        }

        if (argc == 1 || strcmp(argv[1], "--pid") == 0 ||
            strstr(argv[1], "pid") != NULL)
        {
            if (argc == 1 || strcmp(argv[1], "--proc-pid-stat") == 0)
            {
                if (pid("stat") == 1)
                {
                    perror("pid");
                    printf("Failed to read /proc/<pid>/stat\n");
                }
            }

            if (argc == 1 || strcmp(argv[1], "--proc-pid-statm") == 0)
            {
                if (pid("statm") == 1)
                {
                    perror("pid");
                    printf("Failed to read /proc/<pid>/statm\n");
                }
            }

            if (argc == 1 || strcmp(argv[1], "--proc-pid-io") == 0)
            {
                if (pid("io") == 1)
                {
                    perror("pid");
                    printf("Failed to read /proc/<pid>/io\n");
                }
            }
        }
        dequeue();

        if (argc == 1 || strcmp(argv[1], "--sys") == 0 ||
            strcmp(argv[1], "--hwmon") == 0)
        {
            if (sys() == 1)
            {
                perror("sys");
                printf("Failed to read temperatures\n");
            }
            dequeue();
        }

        select(timer + 1, &rfds, NULL, NULL, NULL);   /*auf timer warten*/
        timerfd_settime(timer, 0, &timerValue, NULL); /*timer neu aufsetzen*/
    }
}
