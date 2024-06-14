#include "pacct.h"

int enable_process_accounting(const char *filename) {
        if (acct(filename) == -1) {
                perror("enable_process_accounting failed\n");
                return 1;
        }
        printf("Process accounting enabled and logging to %s\n", filename);
        return 0;
}

int disable_process_accounting() {
        if (acct(NULL) == -1) {
                perror("disable_process_accounting failed\n");
                return 1;
        }
        printf("Process accounting disabled.\n");
        return 0;
}
