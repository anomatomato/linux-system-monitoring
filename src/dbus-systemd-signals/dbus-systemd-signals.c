#include <dbus/dbus.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "mq.h"  // Einbinden der mq Utility

#define MQ_PATH "/dbus"  // Pfad zur Nachrichtenwarteschlange

void monitor_and_report_systemd_events() {
    DBusConnection* conn;
    DBusError err;
    DBusMessage* msg;
    int ret;
    mqd_t mq;

    dbus_error_init(&err);

    /* Initialisiere die Nachrichtenwarteschlange */
    mq = init_mq(MQ_PATH);
    if (mq == -1) {
        exit(1);  // Beende, falls MQ-Initialisierung fehlschlägt
    }

    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Verbindungsfehler (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (conn == NULL) {
        exit(1);
    }

    ret = dbus_bus_request_name(conn, "test.signal.sink", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Namensfehler (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        exit(1);
    }

    dbus_bus_add_match(conn, "type='signal',interface='org.freedesktop.systemd1.Manager'", &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Filterfehler (%s)\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    while (1) {
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            sleep(1);
            continue;
        }

        if (dbus_message_is_signal(msg, "org.freedesktop.systemd1.Manager", "UnitStarted")) {
            char* unit_name;
            if (dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &unit_name, DBUS_TYPE_INVALID)) {
                char message[256];
                snprintf(message, sizeof(message), "Unit %s started", unit_name);
                printf("%s\n", message);
                send_to_mq(message, MQ_PATH);
            }
        }

        dbus_message_unref(msg);
    }

    dbus_connection_close(conn);
    remove_mq(MQ_PATH);
}

int main(int argc, char** argv) {
    monitor_and_report_systemd_events();
    return 0;
}
