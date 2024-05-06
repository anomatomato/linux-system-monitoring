#include "mq.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <time.h>

int send_to_mq(const char* message, const char* mq_path)
{
    mq_unlink(mq_path);
    printf("send_to_mq: %s\n", message);
    struct mq_attr attr;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    mqd_t mq = mq_open(mq_path, (__O_CLOEXEC | O_CREAT | O_RDWR),
                              (S_IRUSR | S_IWUSR), &attr);
    if (mq == -1)
    {
        perror("mq_open failed");
        return -1;
    }

    if (mq_send(mq, message, strlen(message) + 1, 0) == -1)
    {
        perror("mq_send failed");
        mq_close(mq);
        return -1;
    }
    mq_close(mq);
    return 0;
}

long long get_timestamp()
{
    return (long long)time(NULL) * 1000000000;
}