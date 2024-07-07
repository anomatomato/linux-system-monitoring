#include "dbus-systemd-signals.h"
#include <systemd/sd-bus.h>

static int on_systemd_signal(sd_bus_message* m, void* userdata, sd_bus_error* ret_error)
{
    const char *unit, *state;

    unit  = "example.service";
    state = "started";

    char formatted_message[256];
    snprintf(formatted_message, sizeof(formatted_message),
             "systemd_event,unit=%s state=%s %ld", 
             unit, state,
             (long int)time(NULL));

    printf("Event: %s\n", formatted_message);

    return 0;
}

int main(int argc, char* argv[])
{
    sd_bus* bus = NULL;
    int ret;

    ret = sd_bus_open_system(&bus);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-ret));
        exit(EXIT_FAILURE);
    }

    ret = sd_bus_add_match(
        bus, NULL,
        "type='signal',"
        "sender='org.freedesktop.systemd1',"
        "interface='org.freedesktop.systemd1.Manager',"
        "member='UnitNew'",
        on_systemd_signal, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "Failed to add match: %s\n", strerror(-ret));
        sd_bus_close(bus);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        ret = sd_bus_process(bus, NULL);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-ret));
            break;
        }

        if (ret == 0)
        {
            ret = sd_bus_wait(bus, (uint64_t)-1);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-ret));
                break;
            }
        }
    }

    sd_bus_close(bus);
    return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
