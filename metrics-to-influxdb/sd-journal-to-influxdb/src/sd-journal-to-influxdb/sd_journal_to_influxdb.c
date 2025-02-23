#include "sd_journal_to_influxdb.h"

/**
 * @file sd_jourmal_to_influxdb.c
 * @brief This file contains the main implementation of the component sd_journal_to_influxdb.
 */


/**
 * @brief Path to the journal logs.
 */
const char *journal_path = "/var/log/journal/remote";

void set_host(mqtt_config_t *config, char *input) {
        if (strcmp(input, "server") == 0) {
                strncpy(config->host, "tcp://sep-cm0-server.ibr.cs.tu-bs.de:1883/", MAX_HOST_LENGTH);
        } else if (strcmp(input, "localhost") == 0) {
                strncpy(config->host, "tcp://localhost:1883/", MAX_HOST_LENGTH);
        } else {
                fprintf(stderr, "Unknown input: %s\n", input);
        }
}

int journal_to_influxdb(int argc, char *argv[]) {
        /**
         * @brief Configuration for MQTT connection.
         */
        mqtt_config_t config = {
                .port = 1883,                                         // MQTT broker port
                .topic = "linux-monitoring/logs/sd_journal",          // MQTT topic to publish logs
                .client_id = "sd_journal_logs",                       // MQTT client ID
                .qos = 1,                                             // MQTT quality of service level
                .timeout = 10000,                                     // MQTT connection timeout in milliseconds
        };

        printf("Start sending journal to influxdb\n");

        // Disable stdout buffering for testing purpose
        setbuf(stdout, NULL);

        // Handle command-line arguments
        if (argc > 1) {
                set_host(&config, argv[1]);
        } else {
                char input[MAX_HOST_LENGTH];
                printf("Enter host type (server or localhost): ");
                if (fgets(input, sizeof(input), stdin) != NULL) {
                // Remove newline character if present
                size_t len = strlen(input);
                if (len > 0 && input[len-1] == '\n') {
                        input[len-1] = '\0';
                }
                set_host(&config, input);
                } else {
                        fprintf(stderr, "Error reading input.\n");
                        return 1;
                }
        }
        printf("Hostname: %s\n", config.host);

        init_mqtt(&config);

        sd_journal *j;
        char *cursor = NULL;
        int r;

        // Open the specified journal directory
        r = sd_journal_open_directory(&j, journal_path, 0);
        if (r < 0) {
                fprintf(stderr, "Failed to open journal: %s\n", strerror(-r));
                return 1;
        }

        // Seek to the end of the journal initially to only get new entries
        r = sd_journal_seek_tail(j);
        if (r < 0) {
                fprintf(stderr, "Failed to seek to tail: %s\n", strerror(-r));
                sd_journal_close(j);
                return 1;
        }

        sd_journal_next(j);

        while (1) {
                // Wait for new journal entries
                r = sd_journal_wait(j, (uint64_t) -1);
                if (r < 0) {
                        fprintf(stderr, "Failed to wait for journal: %s\n", strerror(-r));
                        sd_journal_close(j);
                        return 1;
                }

                // Iterate through new journal entries
                while (sd_journal_next(j) > 0) {
                        publish_journal_entry(j, &config);

                        // Get the current cursor position
                        r = sd_journal_get_cursor(j, &cursor);
                        if (r < 0) {
                                fprintf(stderr, "Failed to get cursor: %s\n", strerror(-r));
                                free(cursor);
                                sd_journal_close(j);
                                return 1;
                        }
                }

                // Seek to the last read position
                if (cursor) {
                        r = sd_journal_seek_cursor(j, cursor);
                        if (r < 0) {
                                fprintf(stderr, "Failed to seek to cursor: %s\n", strerror(-r));
                                free(cursor);
                                sd_journal_close(j);
                                return 1;
                        }

                        sd_journal_next(j);
                        // Free the cursor string
                        free(cursor);
                        cursor = NULL;
                }
        }

        // clean up
        sd_journal_close(j);
        close_mqtt(&config);
        return 0;
}
