#include "inotify_pacct.h"

int main()
{
    enable_process_accounting(ACCT_FILE);
    monitor_process_accounting();
    return 0;
}