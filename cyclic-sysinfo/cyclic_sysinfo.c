#include <sys/sysinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>        /* For O_* constants */
#include <sys/stat.h>     /* For mode constants */

#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 150

/* Send message in InfluxDB Line Protocol to posix-mq-to-mqtt-bridge */
void send_to_mq(const char* message)
{
    mqd_t mq = mq_open(MQ_PATH, O_WRONLY);
    if (mq == -1)
    {
        perror("mq_open failed");
        exit(-1);
    }

    if (mq_send(mq, message, strlen(message), 0) == -1)
    {
        perror("mq_send failed");
        /* mq_close(mq); */ 
        exit(-1);
    }
}

void gather_sysinfo() {
    struct sysinfo info;

    if (sysinfo(&info) == 0) {
        /* Successfully retrieved system statistics. */
        char message[MAX_MSG_SIZE];
        snprintf(message, MAX_MSG_SIZE + 1, "uptime = %ld, total ram = %lu, free ram = %lu, process count = %hu\n",
               info.uptime, info.totalram, info.freeram, info.procs);        
        send_to_mq(message);
        /* If you need to send this in the Line Protocol format, you'll format it like: */
        /* sysinfo uptime=<value>,total_ram=<value>,free_ram=<value>,process_count=<value> <timestamp> */
    } else {
        /* Handle error. */
        perror("sysinfo call failed");
    }
}


int main(int argc, char *argv[]) {
    while (1) {
        gather_sysinfo();
        /* Sleep for some time before gathering the info again */ 
        /* The sleep duration determines the 'cyclic' nature of the data gathering */
        sleep(60); /* sleep for 1 second. Adjust this value as needed for your use case */
    }
    return 0;
}
