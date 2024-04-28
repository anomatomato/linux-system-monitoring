#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>
#include <time.h>

/* Signal-Handler für Systemd-Events */
static int on_systemd_signal(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    // Nutzdaten extrahieren
    const char *unit, *state;
    
    // Im realen Code müssten Sie die sd_bus_message parsen, um die notwendigen Informationen zu extrahieren.
    // Zum Beispiel so:
    // int r = sd_bus_message_read(m, "ss", &unit, &state);
    // if (r < 0) {
    //     fprintf(stderr, "Failed to parse signal message: %s\n", strerror(-r));
    //     return -1;
    // }

    // Ersatz für die fehlende Parselogik
    unit = "example.service";
    state = "started";
    
    // Ereignis in InfluxDB Line Protocol-Format umwandeln
    char formatted_message[256];
    snprintf(formatted_message, sizeof(formatted_message),
             "systemd_event,unit=%s state=%s %ld", unit, state, (long int)time(NULL));
    
    printf("Ereignis: %s\n", formatted_message);

    return 0; // Signal-Handling erfolgreich
}

int main(int argc, char *argv[]) {
    sd_bus *bus = NULL;
    int ret;

    // Mit dem Systemd-Bus verbinden
    ret = sd_bus_open_system(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-ret));
        exit(EXIT_FAILURE);
    }

    // Auf das Starten/Stoppen von Units abonnieren
    ret = sd_bus_add_match(bus, NULL, "type='signal',sender='org.freedesktop.systemd1',interface='org.freedesktop.systemd1.Manager',member='UnitNew'",
                           on_systemd_signal, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to add match: %s\n", strerror(-ret));
        sd_bus_close(bus);
        exit(EXIT_FAILURE);
    }

    // Die Hauptereignisschleife
    while (1) {
        // Auf neue Ereignisse warten
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-ret));
            break;
        }

        // Keine neuen Ereignisse, warten
        if (ret == 0) {
            ret = sd_bus_wait(bus, (uint64_t) -1);
            if (ret < 0) {
                fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-ret));
                break;
            }
        }
    }

    sd_bus_close(bus);
    return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
