#include "bridge.h"
#include "inotify-coredump.h"
#include <pthread.h>
int main()
{

    pthread_t main_thread;
    pthread_create(&main_thread, NULL, bridge, NULL);
    pthread_t inotify_coredump_thread;
    pthread_create(&inotify_coredump_thread, NULL, inotify_coredump, NULL);
    return 0;
}
