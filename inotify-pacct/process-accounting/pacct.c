#include "pacct.h"

void enable_process_accounting(const char* filename)
{
    if (acct(filename) == -1)
    {
        perror("enable_process_accounting failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Process accounting enabled and logging to %s\n", filename);
}

void disable_process_accounting()
{
    if (acct(NULL) == -1)
    {
        perror("disable_process_accounting failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Process accounting disabled.\n");
}
