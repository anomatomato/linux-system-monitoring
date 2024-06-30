#include "bridge.h"
#include "mq.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

int main() {
        struct epoll_event events[MAX_EVENTS];
        int epid = init_epoll();
        printf("bridge is running...\n");
        MQTTAsync client;
        int rc;
        if ((rc = MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
            MQTTASYNC_SUCCESS) {
                printf("Failed to create client object, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }

        if ((rc = MQTTAsync_setCallbacks(client, NULL, connlost, messageArrived, NULL)) != MQTTASYNC_SUCCESS) {
                printf("Failed to set callback, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }

        /* Connect to the broker once */
        client_msg_t context;
        context.client = &client;
        context.events = events;
        if ((rc = connect_to_broker(&client, events)) != MQTTASYNC_SUCCESS) {
                printf("Failed to connect to broker, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }

        pthread_t inotify_thread, process_msgs_thread;
        client_epoll_t ce;
        ce.client = &client;
        ce.epollfd = epid;
        ce.events = events;
        int rc1 = pthread_create(&inotify_thread, NULL, inotify_mq, (void *) &epid);
        if (rc1 != 0) {
                perror("pthread_create failed");
                exit(EXIT_FAILURE);
        }

        int rc2 = pthread_create(&process_msgs_thread, NULL, process_messages, (void *) &ce);
        if (rc2 != 0) {
                perror("pthread_create failed");
                exit(EXIT_FAILURE);
        }

        while (1)
                usleep(10000L);

        MQTTAsync_destroy(&client);
        return 0;
}