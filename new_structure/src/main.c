#include "bridge.h"
#include "inotify-coredump.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pthread_t bridge_thread, inotify_thread;

    int rc1 = pthread_create(&bridge_thread, NULL, bridge, NULL);
    // int rc2 = pthread_create(&inotify_thread, NULL, inotify_coredump, NULL);

    if (rc1 != 0)
    {
        perror("pthread_create for bridge failed");
        exit(-1);
    }
    // if (rc2 != 0)
    // {
    //     perror("pthread_create for inotify_coredump failed");
    //     exit(-1);
    // }

    while (1)
        ;
    return 0;
}
