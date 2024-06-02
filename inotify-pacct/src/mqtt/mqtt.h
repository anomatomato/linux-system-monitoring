#ifndef MQTT_H
#define MQTT_H

#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  char *host;
  int port;
  char *topic;
  char *client_id;
  int qos;
  long timeout;
} mqtt_config_t;

int init_mqtt(const mqtt_config_t *config);

int publish_mqtt(const mqtt_config_t *config, char *msg);

int close_mqtt(const mqtt_config_t *config);

#endif // MQTT_H
