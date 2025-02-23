#include "publish_influxdb.h"

/**
 * @file publish_influxdb.c
 * @brief This file contains the implementation of publishing data to InfluxDB.
 */

long long get_timestamp() {
        return (long long) time(NULL) * 1000000000;
}

const char *get_field_value(const char *data) {
        const char *equals_sign = strchr(data, '=');
        return equals_sign ? equals_sign + 1 : "Unknown";
}

size_t escape_field_value(const char *field_value, char *output, size_t output_size) {
        if (field_value == NULL || output == NULL || output_size == 0)
                return -1;

        size_t i, j;
        for (i = 0, j = 0; field_value[i] != '\0' && j < output_size - 1; i++) {
                if (field_value[i] == '\\' || field_value[i] == '"')
                        output[j++] = '\\';

                output[j++] = field_value[i];
        }
        output[j] = '\0';
        return j;
}

void publish_journal_entry(sd_journal *j, const mqtt_config_t *config) {
        int r;
        const char *message, *hostname, *syslog_identifier;
        size_t length;
        uint64_t usec;
        time_t sec;
        struct tm *tm_info;
        char time_buffer[32];
        char field_message[MAX_MSG_SIZE];

        // Get the MESSAGE field
        r = sd_journal_get_data(j, "MESSAGE", (const void **) &message, &length);
        if (r < 0) {
                message = "No message";
        } else {
                message = get_field_value(message); // Skip the field name
        }

        /* Escape special characters */
        (void) escape_field_value(message, field_message, sizeof(field_message));

        // Get the _HOSTNAME field
        r = sd_journal_get_data(j, "_HOSTNAME", (const void **) &hostname, &length);
        if (r < 0) {
                hostname = "Unknown";
        } else {
                hostname = get_field_value(hostname); // Skip the field name
        }

        // Get the SYSLOG_IDENTIFIER field
        r = sd_journal_get_data(j, "SYSLOG_IDENTIFIER", (const void **) &syslog_identifier, &length);
        if (r < 0) {
                syslog_identifier = "Unknown";
        } else {
                syslog_identifier = get_field_value(syslog_identifier); // Skip the field name
        }

        // Get the timestamp
        r = sd_journal_get_realtime_usec(j, &usec);
        if (r < 0) {
                strcpy(time_buffer, "Unknown time");
        } else {
                sec = usec / 1000000;
                tm_info = localtime(&sec);
                strftime(time_buffer, sizeof(time_buffer), TIME_FORMAT, tm_info);
        }

        char payload[MAX_MSG_SIZE];
        // Print the formatted journal entry
        printf("%s %s %s: %s\n", time_buffer, hostname, syslog_identifier, field_message);
        snprintf(payload,
                 MAX_MSG_SIZE,
                 "sd-journal,journal_time=%s,pi_name=%s,service=%s message=\"%s\" %lld",
                 time_buffer,
                 hostname,
                 syslog_identifier,
                 field_message,
                 get_timestamp());
        publish_mqtt(config, payload);
}
