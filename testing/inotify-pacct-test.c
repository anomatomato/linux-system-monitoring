#include "inotify_pacct.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

int main()
{
    if (inotify_pacct() == -1)
    {
        perror("inotify_pacct failed");
        return -1;
    }
    return 0;
}