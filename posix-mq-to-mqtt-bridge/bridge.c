#include "MQTTAsync.h"
#include "mq.h"
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#define ADDRESS "tcp://sep-cm0-server.ibr.cs.tu-bs.de:1883/"
#define CLIENTID "bridge"
#define TOPIC "linux-monitoring/stats"
#define QOS 2
#define TIMEOUT 10000L
#define MAX_EVENTS 10
#define WATCH_DIR "/dev/mqueue"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN                                                                \
    (1024 * (EVENT_SIZE + 255 + 1)) /* enough for 1024 events in the buffer */

int finished = 0;

struct client_msg
{
    MQTTAsync* client;
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
    MQTTAsync* client                 = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    printf("Message with token value %d delivery confirmed\n", response->token);
    opts.onSuccess = onDisconnect;
    opts.onFailure = onDisconnectFailure;
    opts.context   = client;
    if ((rc = MQTTAsync_disconnect(*client, &opts)) != MQTTASYNC_SUCCESS)
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
    printf("Successfully connected to broker\n");
    MQTTAsync* client               = (MQTTAsync)context->client;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg       = MQTTAsync_message_initializer;
    int rc;
    opts.onSuccess    = onSend;
    opts.onFailure    = onSendFailure;
    opts.context      = client;
    pubmsg.payload    = context->msg;
    pubmsg.payloadlen = strlen(context->msg);
    pubmsg.payloadlen = strlen(context->msg);
    pubmsg.qos        = QOS;
    pubmsg.retained   = 0;

    printf("Trying to send a message: %s\n", context->msg);

    if ((rc = MQTTAsync_sendMessage(*client, TOPIC, &pubmsg, &opts)) !=
        MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    free(context);
}

int messageArrived(void* context, char* topicName, int topicLen,
                   MQTTAsync_message* m)
{
    // not expecting any messages
    return 1;
}

mqd_t initialize_mq(const char* mq_path)
{
    mqd_t new_queue = mq_open(mq_path, O_RDONLY);
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

int init_epoll()
{
    int epid = epoll_create1(0);
    if (epid == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    return epid;
}

int register_queue(int epid, char* mq_path)
{
    struct epoll_event ev;
    ev.events                           = EPOLLIN;
    char slashed_path[MAX_MSG_SIZE / 2] = "/";
    strcat(slashed_path, mq_path);
    mqd_t new_queue = initialize_mq(slashed_path);
    ev.data.fd      = (int)new_queue;
    if (new_queue == -1)
    {
        perror("mq_open failed");
        return -1;
    }
    epoll_ctl(epid, EPOLL_CTL_ADD, new_queue, &ev);
}

int connect_to_broker(MQTTAsync* client, char* received_msg)
{
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;
    client_msg_t* cm            = (client_msg_t*)malloc(sizeof(client_msg_t));
    cm->client                   = client;
    cm->msg                      = received_msg;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession      = 1;
    conn_opts.onSuccess         = onConnect;
    conn_opts.onFailure         = onConnectFailure;
    conn_opts.context = cm;
    if ((rc = MQTTAsync_connect(*client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    return rc;
}

struct client_epoll
{
    MQTTAsync* client;
    int epollfd;
    struct epoll_event* events;

} typedef client_epoll_t;

void receive_and_push_messages(client_epoll_t* cet)
{
    int epollfd                = cet->epollfd;
    struct epoll_event* events = cet->events;
    for (;;)
    {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n)
        {
            char received_msg[MAX_MSG_SIZE];
            if (mq_receive(events[n].data.fd, received_msg, MAX_MSG_SIZE,
                           NULL) == -1)
            {
                perror("In mq_receive ");
                exit(-1);
            }
            if (connect_to_broker(cet->client, received_msg) == -1)
            {
                perror("connect_to_broker failed");
                exit(EXIT_FAILURE);
            }
            printf("Connecting to broker\n");
        }
    }
}

int inotify_mq(int* epid)
{
    int fd, wd, len;
    char buffer[BUF_LEN];
    printf("watching %s\n", WATCH_DIR);
    /* Initialize inotify instance */
    fd = inotify_init();
    if (fd < 0)
    {
        perror("inotify_init failed");
        exit(EXIT_FAILURE);
    }

    /* Watch WATCH_DIR for new files */
    wd = inotify_add_watch(fd, WATCH_DIR, IN_CREATE);
    if (wd < 0)
    {
        perror("inotify_add_watch failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /* Event loop */
    for (;;)
    {
        /* Read events */
        len = read(fd, buffer, BUF_LEN);
        if (len < 0)
        {
            perror("read failed");
            break;
        }

        /* Process events */
        for (int i = 0; i < len;)
        {
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            if (event->len && event->mask & IN_CREATE)
            {
                printf("message queue /%s added to watchlist\n", event->name);
                register_queue(*epid, event->name);
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}

int main()
{
    struct epoll_event events[MAX_EVENTS];
    int epid = init_epoll();
    printf("bridge is running...\n");
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

    pthread_t broker_thread, inotify_thread;
    client_epoll_t ce;
    ce.client  = &client;
    ce.epollfd = epid;
    ce.events  = events;
    int rc1    = pthread_create(&inotify_thread, NULL, inotify_mq, &epid);
    int rc2 =
        pthread_create(&broker_thread, NULL, receive_and_push_messages, &ce);

    while (!finished)
        usleep(10000L);
    MQTTAsync_destroy(&client);
}