#include "MQTTAsync.h"
#include "mq.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define ADDRESS "tcp://localhost:1883/"
#define CLIENTID "bridge"
#define TOPIC "linux-monitoring/stats"
#define PAYLOAD "Hello World!"
#define QOS 2
#define TIMEOUT 10000L
#define MAX_EVENTS 10
int finished = 0;

struct client_msg
{
    MQTTAsync client;
    char* msg;
} typedef client_msg_t;

void connlost(void* context, char* cause)
{
    MQTTAsync client                   = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);

    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession      = 1;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
}

void onDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Disconnect failed\n");
    finished = 1;
}

void onDisconnect(void* context, MQTTAsync_successData* response)
{
    printf("Successful disconnection\n");
    finished = 1;
}

void onSendFailure(void* context, MQTTAsync_failureData* response)
{
    MQTTAsync client                 = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    printf("Message send failed token %d error code %d\n", response->token,
           response->code);
    opts.onSuccess = onDisconnect;
    opts.onFailure = onDisconnectFailure;
    opts.context   = client;
    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
}

void onSend(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client                 = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    printf("Message with token value %d delivery confirmed\n", response->token);
    opts.onSuccess = onDisconnect;
    opts.onFailure = onDisconnectFailure;
    opts.context   = client;
    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Connect failed, rc %d\n", response ? response->code : 0);
    finished = 1;
}

void onConnect(client_msg_t* context, MQTTAsync_successData* response)
{
    MQTTAsync client               = (MQTTAsync)context->client;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg       = MQTTAsync_message_initializer;
    int rc;

    printf("Successful connection\n");
    opts.onSuccess    = onSend;
    opts.onFailure    = onSendFailure;
    opts.context      = client;
    pubmsg.payload    = context->msg;
    pubmsg.payloadlen = MAX_MSG_SIZE;
    pubmsg.qos        = QOS;
    pubmsg.retained   = 0;

    if ((rc = MQTTAsync_sendMessage(client, TOPIC, &pubmsg, &opts)) !=
        MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
}

int messageArrived(void* context, char* topicName, int topicLen,
                   MQTTAsync_message* m)
{
    // not expecting any messages
    return 1;
}

char* init_mq(char* mq_path)
{
    struct mq_attr attr;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    mqd_t new_queue = mq_open(mq_path, (__O_CLOEXEC | O_CREAT | O_RDWR),
                              (S_IRUSR | S_IWUSR), &attr);

    return new_queue;
}

void add_hostname_to_msg(char* msg)
{
    char hostname[20];
    gethostname(hostname, sizeof(hostname));
    char tag[6 + sizeof(hostname)] = "host=";
    strcat(tag, hostname);
    strcat(msg, " ");
    strcat(msg, tag);
}

int register_all_queues()
{
    int epid = epoll_create1(0);
    if (epid == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        mqd_t new_queue = init_mq(MESSAGE_QUEUES[i]);
        ev.data.fd = (int)new_queue;
        if (new_queue == -1)
        {
            perror("mq_open failed");
            return -1;
        }
        epoll_ctl(epid, EPOLL_CTL_ADD, new_queue, &ev);
    }
    return epid;
}

int connect_to_broker(MQTTAsync client,
                      char* received_msg)
{
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

    int rc;
    client_msg_t cm;
    cm.client                   = client;
    cm.msg                      = received_msg;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession      = 1;
    conn_opts.onSuccess         = onConnect;
    conn_opts.onFailure         = onConnectFailure;

    conn_opts.context = &cm;

    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    return rc;
}

void receive_and_push_messages(MQTTAsync client, int epollfd, struct epoll_event* events)
{
    for (;;)
    {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        printf("wait successfullt waited\n");
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n)
        {
            char received_msg;
            if (mq_receive(events[n].data.fd, &received_msg,
                           MAX_MSG_SIZE, NULL) == -1)
            {
                perror("In mq_receive ");
                exit(-1);
            }
            add_hostname_to_msg(&received_msg);

            printf("received message: %s\n", &received_msg);

            if (connect_to_broker(client, &received_msg) == -1)
            {
                perror("connect_to_broker failed");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void bridge()
{
    char received_msg[MAX_MSG_SIZE + 1];
    struct epoll_event events[MAX_EVENTS];
    int epid = register_all_queues();
    printf("Bridge is running\n");
    MQTTAsync client;

    int rc;
    if ((rc = MQTTAsync_create(&client, ADDRESS, CLIENTID,
                               MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTASYNC_SUCCESS)
    {
        printf("Failed to create client object, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    if ((rc = MQTTAsync_setCallbacks(client, NULL, connlost, messageArrived,
                                     NULL)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callback, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    receive_and_push_messages(client, epid, events);

    while (!finished)
        usleep(10000L);
    MQTTAsync_destroy(&client);
    return rc;
}