#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>


#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 50
int main()
{   
    char hostname[20];
    gethostname(hostname, sizeof(hostname));
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE; 
    // opening a message queue with write/read permission, non blocked( for testing purposes ) with MQ_PATH as its name
    mqd_t new_queue = mq_open(MQ_PATH, (__O_CLOEXEC | O_CREAT | O_RDWR | O_NONBLOCK), (S_IRUSR | S_IWUSR), &attr);
    if (new_queue == -1)
    {
        perror("In mq_open ");
        exit(-1);
    }
    // testing send functionality
    char message[20] = "New Message";
    long priority = 0;
    if (mq_send(new_queue, message, sizeof(message), priority) != 0)
    {
        perror("mq_send failed");
        exit(-1);
    }
    char received_msg[MAX_MSG_SIZE + 1];
    // testing receive functionality
    if (mq_receive(new_queue, received_msg, sizeof(received_msg), NULL) == -1)
    {
        perror("In mq_receive ");
        exit(-1);
    }
    printf("hostname: %s\n", hostname);
    printf("last message with highest priority: %s\n", received_msg);
    return 0;
}