#include "MQTTClient.h"
#include "bridge.h"
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
    // inotify_coredump();
    bridge();
    return 0;
}
