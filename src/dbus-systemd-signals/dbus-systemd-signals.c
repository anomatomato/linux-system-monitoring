#include <dbus/dbus.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void listen_systemd_signals() {
    DBusConnection* conn;
    DBusError err;
    DBusMessage* msg;
    int ret;

    /* Initialisiere die Fehlerbehandlung */
    dbus_error_init(&err);

    /* Verbinde zum D-Bus Systembus */
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Verbindungsfehler (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (conn == NULL) {
        exit(1); /* Beende, falls Verbindung fehlgeschlagen ist */
    }

    /* Fordere einen einzigartigen Namen auf dem Bus an */
    ret = dbus_bus_request_name(conn, "test.signal.sink", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Namensfehler (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        exit(1); /* Beende, wenn wir nicht der primäre Besitzer sind */
    }

    /* Setze einen Filter für die systemd Signale, die uns interessieren */
    dbus_bus_add_match(conn, "type='signal',interface='org.freedesktop.systemd1.Manager'", &err);
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Filterfehler (%s)\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    /* Schleife, um auf Nachrichten zu hören */
    while (1) {
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            sleep(1); /* Warte auf die nächste Nachricht */
            continue;
        }

        /* Überprüfe, ob die Nachricht ein Signal von systemd ist und handle entsprechend */
        if (dbus_message_is_signal(msg, "org.freedesktop.systemd1.Manager", "UnitStarted")) {
            printf("Einheit gestartet\n");
            // Hier würde die weitere Behandlung zur Umwandlung in das Line Protocol stattfinden
        }

        /* Gebe das Nachrichtenobjekt frei, um Speicher freizugeben */
        dbus_message_unref(msg);
    }

    /* Schließe die DBus-Verbindung, wenn fertig */
    dbus_connection_close(conn);
}

int main(int argc, char** argv) {
    listen_systemd_signals(); /* Starte die Funktion zum Abhören von Signalen */
    return 0;
}
