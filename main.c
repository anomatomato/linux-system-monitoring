#include "bridge.h"
#include "inotify-coredump.h"
#include <pthread.h>
int main()
{

    pthread_t main_thread;

    printf("Before creation\n");
    int result = pthread_create(&main_thread, NULL, bridge, NULL);
    printf("After creation\n");

    if (result != 0)
    {
        perror("pthread_create failed");
        exit(-1);
    }

    pthread_join(main_thread, NULL);
    // pthread_t inotify_coredump_thread;
    // pthread_create(&inotify_coredump_thread, NULL, inotify_coredump, NULL);
    printf("Last creation\n");

    return 0;
}
