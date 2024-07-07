#include <fcntl.h> /* Für O_* Konstanten, wie O_RDONLY, die in der open Systemfunktion verwendet werden */
#include "../utilities/mq.h" /* Benutzerdefinierte Utility für die Verwaltung von Nachrichtenwarteschlangen */
#include <stdio.h> /* Standard-Header für Ein-/Ausgabeoperationen für Funktionen wie printf, snprintf, perror */
#include <stdlib.h> /* Standardbibliothek für Speicherallokation, Prozesskontrolle und Konversionen */
#include <string.h> /* String-Handling-Funktionen wie strtok und strncpy */
#include <sys/stat.h> /* Für Moduskonstanten und Dateiberechtigungen */
#ifdef __linux__
#include <sys/sysinfo.h> /* Einbinden von sysinfo Struktur und Funktionsdeklarationen */
#else
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#include <unistd.h> /* Für POSIX Konstanten und Funktionen wie sleep, close und read */
#include <time.h> /* Für Uhr- und Timerfunktionen */
#include <sys/timerfd.h> /* Für Timer-Dateideskriptor-Funktionen und Konstanten */
#include <sys/epoll.h> /* Für epoll Funktionen und Konstanten */
#include <mqueue.h> /* Für POSIX Nachrichtenwarteschlangen */

#define MQ_PATH "/sysinfo" /* Pfad für die Nachrichtenwarteschlange */
//#define CYCLE_INTERVAL 5 /* Intervall in Sekunden zur periodischen Erfassung der Systeminformationen */
#define MAX_EVENTS 10 

void gather_sysinfo(int v) {
#ifdef __linux__
    struct sysinfo info; /* Struktur zum Halten der Systemstatistiken */
    if (sysinfo(&info) == 0) { /* Erfolgreiche Erfassung der Systeminformationen */
        char message[256]; /* Puffer, um die zu sendende Nachricht zu halten */
        /* Formatieren der Systeminformationen in InfluxDB Line Protocol */
        snprintf(message, sizeof(message),
                 "sysinfo,host=my_host uptime=%ld,total_ram=%lu,free_ram=%lu,process_count=%hu %ld",
                 info.uptime, /* System-Uptime in Sekunden */
                 info.totalram * info.mem_unit, /* Gesamte nutzbare Hauptspeichergröße */
                 info.freeram * info.mem_unit, /* Verfügbare Speichergröße */
                 info.procs, /* Anzahl der aktuellen Prozesse */
                 (long)time(NULL) * 1000000000); /* Zeitstempel in Nanosekunden */
        if (v)
            printf("Statistiken: %s\n", message); /* Drucken der gesammelten Informationen zur Demonstration */

        /* Nachricht an die Nachrichtenwarteschlange senden */
        send_to_mq(message, MQ_PATH);
    } else {
        perror("sysinfo Aufruf fehlgeschlagen"); /* Fehler ausgeben, wenn der sysinfo-Aufruf fehlschlägt */
    }
#else
    int mib[2];
    int64_t physical_memory;
    size_t length = sizeof(physical_memory);

    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;

    if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0) {
        char message[256]; /* Puffer, um die zu sendende Nachricht zu halten */
        /* Formatieren der Systeminformationen in InfluxDB Line Protocol */
        snprintf(message, sizeof(message),
                 "sysinfo,host=my_host total_ram=%lld %ld",
                 physical_memory, /* Gesamte nutzbare Hauptspeichergröße */
                 (long)time(NULL) * 1000000000); /* Zeitstempel in Nanosekunden */
        if (v)
            printf("Statistiken: %s\n", message); /* Drucken der gesammelten Informationen zur Demonstration */

        /* Nachricht an die Nachrichtenwarteschlange senden */
        send_to_mq(message, MQ_PATH);
    } else {
        perror("sysctl Aufruf fehlgeschlagen"); /* Fehler ausgeben, wenn der sysctl-Aufruf fehlschlägt */
    }
#endif
}

int main(int argc, char *argv[]) {
    int opt;
    int c = 0;
    int v = 0;
    while (1) {
        opt = getopt(argc, argv, "vc:");
        if (opt == -1)
            break;
        switch (opt) {
        case 'c':
            sscanf(optarg, "%d", &c);
            break;
        case 'v':
            v = 1;
            break;
        }
    }

    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1) {
        perror("timerfd_create fehlgeschlagen"); /* Fehler behandeln, wenn die Timererstellung fehlschlägt */
        exit(EXIT_FAILURE); /* Mit einem Fehlerergebnis beenden */
    }

    struct itimerspec timerValue;
    timerValue.it_value.tv_sec = c; /* Erstes Ablaufen nach CYCLE_INTERVAL Sekunden */
    timerValue.it_value.tv_nsec = 0; /* Keine ersten Ablauf-Nanosekunden */
    timerValue.it_interval.tv_sec = c; /* Timerintervall in Sekunden */
    timerValue.it_interval.tv_nsec = 0; /* Timerintervall in Nanosekunden */

    if (timerfd_settime(fd, 0, &timerValue, NULL) == -1) {
        perror("timerfd_settime fehlgeschlagen"); /* Fehler behandeln, wenn das Einstellen des Timers fehlschlägt */
        close(fd); /* Den Timer-Dateideskriptor schließen */
        exit(EXIT_FAILURE); /* Mit einem Fehlerergebnis beenden */
    }

    fd_set rfds; /* Tracking Timer */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    FD_SET(fd, &rfds);

    int epoll_fd = epoll_create1(0); /* Keine Flags sind angegeben (0), Standard-Einstellungen verwenden */
    if (epoll_fd == -1) {
        perror("epoll_create fehlgeschlagen"); /* Fehler behandeln, wenn die epoll-Erstellung fehlschlägt */
        close(fd); /* Den Timer-Dateideskriptor schließen */
        exit(EXIT_FAILURE); /* Mit einem Fehlerergebnis beenden */
    }

    struct epoll_event ev;
    ev.events = EPOLLIN; /* Interessiert am eingabebereiten Ereignis (EPOLLIN) */
    ev.data.fd = fd; /* Den Timer-Dateideskriptor mit dem Ereignis verknüpfen */

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl fehlgeschlagen"); /* Fehler behandeln, wenn der epoll-Steuervorgang fehlschlägt */
        close(epoll_fd); /* Den epoll-Dateideskriptor schließen */
        close(fd); /* Den Timer-Dateideskriptor schließen */
        exit(EXIT_FAILURE); /* Mit einem Fehlerergebnis beenden */
    }
    if (init_mq(MQ_PATH) == -1) /* MQ für die Bridge erstellen */
            return 1;
    struct epoll_event events[MAX_EVENTS];
    while (1) { /* Endlosschleife zur Behandlung von Ereignissen, wenn sie eintreffen */
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait fehlgeschlagen"); /* Fehler behandeln, wenn das epoll-Warten fehlschlägt */
            break; /* Schleife unterbrechen, um zu beenden */
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == fd) { /* Überprüfen, ob das Ereignis vom Timer stammt */
                uint64_t expirations; /* Variable zum Zählen, wie oft der Timer abgelaufen ist */
                read(fd, &expirations, sizeof(expirations)); /* Lesen, um das Ereignis zu löschen */
                gather_sysinfo(v); /* Funktion gather_sysinfo aufrufen, um Systeminfos zu sammeln und zu drucken */
            }
        }

        if (c == 0) {/* Wenn c==0, wird das alles nur einmal gemacht */
            close(fd);
            return 0;
        }

        select(fd + 1, &rfds, NULL, NULL, NULL);   /* Warten auf Timerablauf */
        timerfd_settime(fd, 0, &timerValue, NULL); /* Timer zurücksetzen */

    }

    close(epoll_fd); /* Den epoll-Dateideskriptor schließen */
    close(fd); /* Den Timer-Dateideskriptor schließen */
    return 0; /* Erfolg zurückgeben */
}
