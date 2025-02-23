#ifndef BRIDGE_H
#define BRIDGE_H

#define ADDRESS "tcp://sep-cm0-server.ibr.cs.tu-bs.de:1883/"
#define TOPIC "linux-monitoring/stats"
#define QOS 2
#define TIMEOUT 10000L
#define MAX_EVENTS 10
#define WATCH_DIR "/dev/mqueue"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 255 + 1)) /* enough for 1024 events in the buffer */

#include "MQTTAsync.h"
#include "mqueue.h"

struct client_msg {
        MQTTAsync *client;
        struct epoll_event *events;
} typedef client_msg_t;

struct client_epoll {
        MQTTAsync *client;
        int epollfd;
        struct epoll_event *events;
} typedef client_epoll_t;

void add_hostname_to_msg(char *msg);
void *inotify_mq(void *arg);
void *process_messages(void *arg);
int register_queue(int epid, const char *mq_path);
mqd_t initialize_mq(const char *mq_path);
int init_epoll();
MQTTAsync init_MQTT_client();
int connect_to_broker(MQTTAsync *client);
int send_message_to_broker(MQTTAsync *client, char *received_msg);
void *process_messages(void *arg);

void connlost(void *context, char *cause);
int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *m);

#endif
