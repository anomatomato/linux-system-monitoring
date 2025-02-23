#ifndef PUBLISH_TO_INFLUXDB_H
#define PUBLISH_TO_INFLUXDB_H

#include "mqtt.h"
#include <systemd/sd-journal.h>

#define MAX_MSG_SIZE 2048            /**< The maximum size of the message to be sent to InfluxDB. */
#define TIME_FORMAT "%b-%d-%H:%M:%S" /**< The format of the timestamp used in the message. */

/**
 * Returns the current timestamp in nanoseconds.
 *
 * @return The current timestamp in nanoseconds.
 */
long long get_timestamp();

/**
 * Retrieves the value of a field from a string of data.
 *
 * This function searches for the first occurrence of the '=' character in the given data string.
 * If found, it returns a pointer to the character immediately after the '=' character, which represents the field value.
 * If the '=' character is not found, it returns the string "Unknown" as the default field value.
 *
 * @param data The string of data containing the field and its value.
 * @return A pointer to the field value, or "Unknown" if the field value is not found.
 */
const char *get_field_value(const char *data);

/**
 * Modifies a journal entry in line protocol form and send it to InfluxDB using MQTT.
 *
 * @param j The sd_journal object representing the journal entry.
 * @param config The MQTT configuration.
 */
void publish_journal_entry(sd_journal *j, const mqtt_config_t *config);

#endif // PUBLISH_TO_INFLUXDB_H
