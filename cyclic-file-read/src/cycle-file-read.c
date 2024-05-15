#include <stdio.h>
#include "cyclic-file-read.h"
#include "stat.h"
#include "net.h"
#include "disk.h"
#include "pid.h"
#include "sys.h"
#include "common.h"
#include "../../utilities/mq.h"

int cfr() {
        if (init_mq("/cfr") == -1)
                return 1;

        if (stat() == 1) {
                perror("stat");
                printf("Failed to read /proc/stat\n");
        }
        if (net() == 1) {
                perror("net");
                printf("Failed to read /proc/net/dev\n");
        }
        if (disk() == 1) {
                perror("disk");
                printf("Failed to read /proc/diskstats\n");
        }
        /*if (pid("stat") == 1) {
                perror("pid");
                printf("Failed to read /proc/<pid>/stat\n");
        }
        if (pid("statm") == 1) {
                perror("pid");
                printf("Failed to read /proc/<pid>/statm\n");
        }*/
        //sys();

        dequeue();

        return 0;
}
