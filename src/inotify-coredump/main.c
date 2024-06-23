#define _XOPEN_SOURCE 700

#include "inotify-coredump.h"
#include "signal_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *prog) {
        printf("Usage: %s [FLAG]\n", prog);
        printf("  -t    test mode\n");
        printf("  -v    verbose, print Line Protocol lines\n");
        printf("  -h    display this help message\n");
}
int main(int argc, char *argv[]) {
        int opt;
        int test = 0;
        int verbose = 0;

        /* Loop over all flag options */
        while ((opt = getopt(argc, argv, "tvh")) != -1) {
                switch (opt) {
                case 't':
                        test = 1;
                        printf("Test\n");
                        break;
                case 'v':
                        verbose = 1;
                        printf("Verbose\n");
                        break;
                case 'h':
                        print_usage(argv[0]);
                        exit(EXIT_SUCCESS);

                case '?':
                        print_usage(argv[0]);
                        exit(EXIT_FAILURE);
                default:
                        break;
                }
        }

        if (setup_signal_handling() == -1) {
                fprintf(stderr, "setup_signal_handling failed\n");
                exit(EXIT_FAILURE);
        }

        /* Start inotify_coredump */
        if (inotify_coredump(test, verbose) == -1) {
                fprintf(stderr, "inotify_coredump failed\n");
                exit(EXIT_FAILURE);
        }

        printf("inotify-coredump exited successfully.\n");
        return 0;
}