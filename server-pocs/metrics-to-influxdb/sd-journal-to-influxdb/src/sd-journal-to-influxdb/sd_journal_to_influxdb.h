#ifndef JOURNAL_TO_INFLUXDB_H
#define JOURNAL_TO_INFLUXDB_H

#include "publish_influxdb.h"

/**
 * Sets the MQTT host address based on the provided input.
 *
 * This function updates the host field of the provided mqtt_config_t structure based on the input string.
 * If the input is "server", the function sets the host to the predefined server address
 * If the input is "localhost", the function sets the host to the localhost address.
 *
 * @param config Pointer to an mqtt_config_t structure containing the MQTT configuration settings.
 * @param input A string that determines which host address to set. Valid inputs are "server" and "localhost".
 */
void set_host(mqtt_config_t *config, char *input);

/**
 * @brief Sends journal entries to InfluxDB using MQTT.
 *
 * This function configures the MQTT connection and starts reading systemd journal entries.
 * It initially seeks to the end of the journal to process only new entries.
 * When new entries are detected, they are published to InfluxDB via MQTT.
 * The function continues to monitor and publish new journal entries indefinitely.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments. The first argument can be used to specify the host type ("server" for running  or "localhost" for testing).
 * @return a non-zero error code on failure.
 */
int journal_to_influxdb(int argc, char *argv[]);

#endif // JOURNAL_TO_INFLUXDB_H
