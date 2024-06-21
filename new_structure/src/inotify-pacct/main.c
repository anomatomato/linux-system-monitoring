#include "inotify_pacct.h"

int main() {
        if (enable_process_accounting(ACCT_FILE) != 0) {
                perror("Failed in enabling process accounting\n");
                return 1;
        }
        if (monitor_process_accounting() != 0) {
                perror("Failed in monitoring process accounting\n");
                return 2;
        }
        if (disable_process_accounting() != 0) {
                perror("Failed in disabling process accounting\n");
                return 3;
        }
        return 0;
}
