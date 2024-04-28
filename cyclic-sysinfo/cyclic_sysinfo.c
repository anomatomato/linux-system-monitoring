#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void gather_sysinfo() {
    struct sysinfo info;

    if (sysinfo(&info) == 0) {
        // Successfully retrieved system statistics.
        // Now you can print or process the information.
        printf("uptime = %ld, total ram = %lu, free ram = %lu, process count = %hu\n",
               info.uptime, info.totalram, info.freeram, info.procs);
        // If you need to send this in the Line Protocol format, you'll format it like:
        // sysinfo uptime=<value>,total_ram=<value>,free_ram=<value>,process_count=<value> <timestamp>
    } else {
        // Handle error.
        perror("sysinfo call failed");
    }
}


int main(int argc, char *argv[]) {
    while (1) {
        gather_sysinfo();
        // Sleep for some time before gathering the info again
        // The sleep duration determines the 'cyclic' nature of the data gathering
        sleep(1); // sleep for 1 second. Adjust this value as needed for your use case
    }
    return 0;
}
