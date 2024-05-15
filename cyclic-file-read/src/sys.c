#include <stdio.h>
#include <string.h>
#include <sensors/sensors.h>
#include"sys.h"

void sys() {
        FILE* sysf;
        if ((sysf = fopen("/etc/sensor3.conf", "r")) == NULL) {
                return;
        }
        sensors_init(sysf);
        sensors_cleanup();
}
