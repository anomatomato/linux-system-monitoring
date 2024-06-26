#ifndef DBUS_SYSTEMD_SIGNALS_H
#define DBUS_SYSTEMD_SIGNALS_H

#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>
#include <time.h>

static int on_systemd_signal(sd_bus_message* m, void* userdata, sd_bus_error* ret_error);
int main(int argc, char* argv[]);

#endif // DBUS_SYSTEMD_SIGNALS_H
