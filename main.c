#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define MQ_PATH "/my_queue"
#define MAX_MSG_SIZE 50
#define ADDRESS "tcp://localhost:1883/"
#define CLIENTID "ExampleClientPub"
#define TIMEOUT 10000L

int main()
{   
    mq_unlink(MQ_PATH);
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

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
                                MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    if (mq_receive(new_queue, received_msg, sizeof(received_msg), NULL) == -1)
    {
        perror("In mq_receive ");
        exit(-1);
    }

    pubmsg.payload = received_msg;
    pubmsg.payloadlen = (int)strlen(received_msg);
    pubmsg.qos = 2;
    pubmsg.retained = 0;

    char* topic = "test";

    if ((rc = MQTTClient_publishMessage(client, topic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to publish message, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    MQTTClient_message* mqtt_response;
    int topic_len = (int)strlen(topic);
    MQTTClient_receive(&client, &topic, &topic_len, &mqtt_response, TIMEOUT);
    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
        printf("Failed to disconnect, return code %d\n", rc);
    MQTTClient_destroy(&client);

    // testing receive functionality
    printf("hostname: %s\n", hostname);
    printf("last message with highest priority: %s\n", received_msg);
    printf("mqtt message %p\n", mqtt_response);
    return 0;
}