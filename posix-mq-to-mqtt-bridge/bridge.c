#include "../utilities/mq.h"
#include "MQTTAsync.h"
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ADDRESS "tcp://localhost:1883/"
#define CLIENTID "ExampleClientPub"
#define TOPIC "linux-monitoring/stats"
#define PAYLOAD "Hello World!"
#define QOS 2
#define TIMEOUT 10000L

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
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
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

int bridge()
{
    char hostname[20];
    gethostname(hostname, sizeof(hostname));
    struct mq_attr attr;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    char received_msg[MAX_MSG_SIZE + 1];
    mqd_t new_queue =
        mq_open(MQ_PATH, (__O_CLOEXEC | O_CREAT | O_RDWR | O_NONBLOCK),
                (S_IRUSR | S_IWUSR), &attr);
    if (mq_receive(new_queue, received_msg, sizeof(received_msg), NULL) == -1)
    {
        perror("In mq_receive ");
        exit(-1);
    }
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
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

    printf("Waiting for publication of %s\n"
           "on topic %s for client with ClientID: %s\n",
           received_msg, TOPIC, CLIENTID);

    while (!finished)
        usleep(10000L);
    MQTTAsync_destroy(&client);
    return rc;
}