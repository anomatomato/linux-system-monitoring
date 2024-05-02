#include "mq.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */

/* Send message in InfluxDB Line Protocol to posix-mq-to-mqtt-bridge */
int send_to_mq(const char* message)
{
    //! Test
    printf("send_to_mq: %s\n", message);

    // for testing the creation and writing of the message queue
    //! mqd_t mq = mq_open(MQ_PATH, O_WRONLY | O_CREAT, 0666, NULL);

    mqd_t mq = mq_open(MQ_PATH, O_WRONLY);
    if (mq == -1)
    {
        return -1;
    }

    if (mq_send(mq, message, strlen(message), 0) == -1)
    {
        mq_close(mq);
        return -1;
    }
    mq_close(mq);
    return 0;
}