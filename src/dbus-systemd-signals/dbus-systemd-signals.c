#include "mq.h"
#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQ_PATH "/dbus"

mqd_t initialize_message_queue() {
        mqd_t mq = init_mq(MQ_PATH);
        if (mq == -1) {
                perror("Message queue initialization failed");
                exit(1);
        }
        return mq;
}

DBusConnection *connect_to_dbus_system_bus() {
        DBusError err;
        dbus_error_init(&err);

        DBusConnection *connection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
        if (dbus_error_is_set(&err)) {
                fprintf(stderr, "Connection error (%s)\n", err.message);
                dbus_error_free(&err);
        }
        if (connection == NULL) {
                fprintf(stderr, "Failed to get DBus connection.\n");
                exit(1);
        }
        return connection;
}

/* Add match rule to monitor systemd events */
void add_dbus_match(DBusConnection *connection) {
        DBusError err;
        dbus_error_init(&err);

        dbus_bus_add_match(connection, "type='signal',interface='org.freedesktop.systemd1.Manager'", &err);
        if (dbus_error_is_set(&err)) {
                fprintf(stderr, "Match rule error (%s)\n", err.message);
                dbus_error_free(&err);
                exit(1);
        }
}

void handle_job_new_signal(DBusMessage *msg, mqd_t mq, int v) {
        DBusMessageIter args;
        if (dbus_message_iter_init(msg, &args)) {
                if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_UINT32) {
                        int job_id;
                        dbus_message_iter_get_basic(&args, &job_id);
                        dbus_message_iter_next(&args);

                        if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_OBJECT_PATH) {
                                const char *job_path;
                                dbus_message_iter_get_basic(&args, &job_path);
                                dbus_message_iter_next(&args);

                                if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING) {
                                        char *unit_name;
                                        dbus_message_iter_get_basic(&args, &unit_name);
                                        char message[256];
                                        snprintf(message,
                                                 sizeof(message),
                                                 "systemdSignal,job=new unit=\"%s\",id=%u",
                                                 unit_name,
                                                 job_id);
                                        if (v)
                                                printf("%s\n", message);
                                        send_to_mq(message, MQ_PATH);
                                }
                        }
                }
        }
}

void handle_job_removed_signal(DBusMessage *msg, mqd_t mq, int v) {
        DBusMessageIter args;
        if (dbus_message_iter_init(msg, &args)) {
                if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_UINT32) {
                        int job_id;
                        dbus_message_iter_get_basic(&args, &job_id);
                        dbus_message_iter_next(&args);

                        if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_OBJECT_PATH) {
                                const char *job_path;
                                dbus_message_iter_get_basic(&args, &job_path);
                                dbus_message_iter_next(&args);

                                if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING) {
                                        char *result;
                                        dbus_message_iter_get_basic(&args, &result);
                                        char message[256];
                                        snprintf(message,
                                                 sizeof(message),
                                                 "systemdSignal,job=removed id=%u,result=\"%s\"",
                                                 job_id,
                                                 result);
                                        if (v)
                                                printf("%s\n", message);
                                        send_to_mq(message, MQ_PATH);
                                }
                        }
                }
        }
}

void process_dbus_messages(DBusConnection *connection, mqd_t mq, int v) {
        while (1) {
                dbus_connection_read_write(connection, 0);
                DBusMessage *msg = dbus_connection_pop_message(connection);

                if (msg == NULL) {
                        sleep(2);
                        continue;
                }

                const char *interface = dbus_message_get_interface(msg);
                const char *member = dbus_message_get_member(msg);

                if (strcmp(interface, "org.freedesktop.systemd1.Manager") == 0) {
                        if (strcmp(member, "JobNew") == 0) {
                                handle_job_new_signal(msg, mq, v);
                        } else if (strcmp(member, "JobRemoved") == 0) {
                                handle_job_removed_signal(msg, mq, v);
                        } else {
                        }
                }
                dbus_message_unref(msg);
        }
}

/* Main running function for monitoring and reporting systemd events */
void monitor_and_report_systemd_events(int v) {
        mqd_t mq = initialize_message_queue();
        DBusConnection *connection = connect_to_dbus_system_bus();
        add_dbus_match(connection);
        process_dbus_messages(connection, mq, v);
        dbus_connection_close(connection);
        remove_mq(MQ_PATH);
}

int main(int argc, char **argv) {
        int v = 0;
        /* Loop over all flag options */
        while (1) {
                int opt;
                opt = getopt(argc, argv, "v");
                if (opt == -1)
                        break;
                switch (opt) {
                case 'v':
                        v = 1;
                        break;
                default:
                        break;
                }
        }

        monitor_and_report_systemd_events(v);
        return 0;
}