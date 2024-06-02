#include "mqtt.h"

MQTTClient client;

int init_mqtt(const mqtt_config_t *config) {
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;

  MQTTClient_create(&client, config->host, config->client_id,
                    MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 60;
  conn_opts.cleansession = 1;

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
    printf("Failed to connect, return code %d\n", rc);
    return 1;
  }
  printf("Connected to MQTT broker at %s\n", config->host);
  return 0;
}

int publish_mqtt(const mqtt_config_t *config, char *msg) {
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  pubmsg.payload = msg;
  pubmsg.payloadlen = strlen(msg);
  pubmsg.qos = config->qos;
  pubmsg.retained = 0;
  int rc;
  do {
    MQTTClient_publishMessage(client, config->topic, &pubmsg, &token);
    printf("Waiting for publication of message with token %d\n", token);

    rc = MQTTClient_waitForCompletion(client, token, config->timeout);
    if (rc == MQTTCLIENT_DISCONNECTED) {
      printf("MQTT client disconnected. Trying to reconnect...\n");
      init_mqtt(config);
    } else if (rc == MQTTCLIENT_FAILURE) {
      printf("Failed to deliver message, return code %d\n", rc);
      return -1;
    }
  } while (rc != MQTTCLIENT_SUCCESS);
  sleep(1);
  printf("Message with token %d delivered\n", token);
  return 0;
}

int close_mqtt(const mqtt_config_t *config) {
  MQTTClient_disconnect(client, config->timeout);
  MQTTClient_destroy(&client);
  return 0;
}
