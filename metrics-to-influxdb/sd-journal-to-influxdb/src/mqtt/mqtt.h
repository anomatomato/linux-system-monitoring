#ifndef MQTT_H
#define MQTT_H

#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_HOST_LENGTH 100

/**
 * @brief Structure representing the MQTT configuration.
 *
 * This structure holds the configuration parameters for MQTT connection.
 */
typedef struct {
        char host[MAX_HOST_LENGTH];      /**< MQTT broker host address */
        int port;                        /**< MQTT broker port number */
        char topic[MAX_HOST_LENGTH];     /**< MQTT topic to subscribe/publish */
        char client_id[MAX_HOST_LENGTH]; /**< MQTT client identifier */
        int qos;                         /**< MQTT quality of service level */
        long timeout;                    /**< MQTT connection timeout in milliseconds */
} mqtt_config_t;

/**
 * Initializes the MQTT client and connects to the MQTT broker.
 *
 * @param config The MQTT configuration settings.
 * @return 0 if the MQTT client is successfully initialized and connected,
 *         1 if there was an error connecting to the MQTT broker.
 */
int init_mqtt(const mqtt_config_t *config);

/**
 * Publishes an MQTT message with the given configuration and message.
 *
 * @param config The MQTT configuration.
 * @param msg The message to be published.
 * @return 0 on success, -1 on failure.
 */
int publish_mqtt(const mqtt_config_t *config, char *msg);

/**
 * Closes the MQTT client connection.
 *
 * @param config The MQTT configuration settings.
 * @return 0 if the MQTT client is successfully closed,
 *         1 if there was an error closing the MQTT client.
 */
int close_mqtt(const mqtt_config_t *config);

#endif // MQTT_H
