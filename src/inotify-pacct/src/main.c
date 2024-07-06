#include "inotify_pacct.h"

void print_usage(const char *prog) {
        printf("Usage: %s [FLAG]\n", prog);
        printf("  -v    verbose, print Line Protocol lines\n");
        printf("  -h    display this help message\n");
}

int main(int argc, char *argv[]) {
        process_monitor_t monitor = {};
        int opt;

        /* Loop over all flag options */
        while ((opt = getopt(argc, argv, "vh")) != -1) {
                switch (opt) {
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

        if (enable_process_accounting(ACCT_FILE) != 0) {
                perror("Failed in enabling process accounting\n");
                return 1;
        }

        if (monitor_process_accounting(&monitor) != 0) {
                perror("Failed in monitoring process accounting\n");
                return 2;
        }
        
        if (disable_process_accounting() != 0) {
                perror("Failed in disabling process accounting\n");
                return 3;
        }
        return 0;
}
