#include "inotify-coredump.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

int main()
{
    if (inotify_coredump() == -1)
    {
        perror("inotify_coredump failed");
        return -1;
    }
    return 0;
}
