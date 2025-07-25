#include "mq.h"
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <time.h>

int init_mq(const char *mq_path) {
        mq_unlink(mq_path);
        printf("Creating message queue: %s\n", mq_path);

        /* Set attributes for the message queue*/
        struct mq_attr attr;
        attr.mq_maxmsg = MAX_MESSAGES;
        attr.mq_msgsize = MAX_MSG_SIZE;

        /* Create the message queue */
        mqd_t mq = mq_open(mq_path, O_CREAT, (S_IRUSR | S_IWUSR), &attr);
        if (mq == -1) {
                perror("mq_open failed");
                return -1;
        }
        return mq;
}

int send_to_mq(const char *message, const char *mq_path) {
        /* Open the message queue for writing */
        mqd_t mq = mq_open(mq_path, O_WRONLY);
        if (mq == -1) {
                perror("mq_open failed");
                return -1;
        }

        /* Send message to bridge */
        if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
                perror("mq_send failed");
                mq_close(mq);
                return -1;
        }

        // printf("Sent message to MQ %s: %s\n", mq_path, message);
        mq_close(mq);
        return 0;
}

void remove_mq(const char *mq_path) {
        printf("Removing Message Queue: %s\n", mq_path);
        (void) mq_unlink(mq_path);
        return;
}

long long get_timestamp() {
        return (long long) time(NULL) * 1000000000;
}
