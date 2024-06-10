#define _XOPEN_SOURCE 700

#include "inotify-coredump.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
        /* Register signal handler */
        struct sigaction sa;
        sa.sa_handler = handle_signal;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("sigaction failed");
                exit(EXIT_FAILURE);
        }

        if (sigaction(SIGTERM, &sa, NULL) == -1) {
                perror("sigaction failed");
                exit(EXIT_FAILURE);
        }

        if (sigaction(SIGHUP, &sa, NULL) == -1) {
                perror("sigaction failed");
                exit(EXIT_FAILURE);
        }


        if (sigaction(SIGQUIT, &sa, NULL) == -1) {
                perror("sigaction");
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