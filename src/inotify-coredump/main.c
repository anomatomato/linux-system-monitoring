#define _XOPEN_SOURCE 700

#include "colored_output.h"
#include "inotify-coredump.h"
#include "signal_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *prog) {
        printf("Usage: %s [FLAG]\n", prog);
        printf("  -v    verbose, print Line Protocol lines\n");
        printf("  -h    display this help message\n");
}

int main(int argc, char *argv[]) {
        coredump_monitor_t monitor;
        int opt;

        /* Loop over all flag options */
        while ((opt = getopt(argc, argv, "tvh")) != -1) {
                switch (opt) {
                case 't':
                        break;
                case 'v':
                        monitor.flags = VERBOSE;
                        break;
                case 'h':
                        print_usage(argv[0]);
                        return 0;

                case '?':
                        print_usage(argv[0]);
                        return -1;
                default:
                        break;
                }
        }

        if (setup_signal_handling() == -1) {
                fprintf(stderr, "setup_signal_handling failed\n");
                return -1;
        }

        /* Start inotify_coredump */
        if (inotify_coredump(&monitor) == -1) {
                fprintf(stderr, COLOR_RED STYLE_BOLD "Test failed" RESET_ALL "\n");
                return -1;
        }

        printf(COLOR_GREEN STYLE_BOLD "inotify-coredump exited successfully." RESET_ALL "\n");
        return 0;
}