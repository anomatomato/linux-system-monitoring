#define _XOPEN_SOURCE 700

#include "inotify-coredump.h"
#include "signal_handling.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
        if (setup_signal_handling() == -1) {
                fprintf(stderr, "setup_signal_handling failed\n");
                exit(EXIT_FAILURE);
        }

        /* Start inotify_coredump */
        if (inotify_coredump() == -1) {
                fprintf(stderr, "inotify_coredump failed\n");
                exit(EXIT_FAILURE);
        }

        printf("inotify-coredump exited successfully.\n");
        return 0;
}