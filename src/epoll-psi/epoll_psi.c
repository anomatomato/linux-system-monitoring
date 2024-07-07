#include <fcntl.h>
#include "../utilities/mq.h"    /* Einbinden der Hilfsbibliothek für Nachrichtenwarteschlangen zur Interprozesskommunikation */
#include <mqueue.h>             /* Einbinden der POSIX Nachrichtenwarteschlangen-Header */
#include <stdio.h>              /* Standard-I/O-Operationen */
#include <stdlib.h>             /* Standardbibliothek für Speicherallokation, Prozesssteuerung, Konvertierungen usw. */
#include <string.h>             /* Stringoperationen wie strtok, strncpy usw. */
#include <sys/epoll.h>          /* Epoll eventgesteuerte I/O für die Verwaltung einer großen Anzahl von Dateideskriptoren */
#include <time.h>               /* Zeitfunktionen */
#include <unistd.h>             /* POSIX Betriebssystem API */

//#define MAX_EVENTS 4            /* Maximale Anzahl von Ereignissen, die gleichzeitig von epoll_wait verarbeitet werden */
#define BUFFER_SIZE 1024        /* Puffergröße für das Lesen von Daten */
#define MQ_PATH "/psi"     /* Pfad zur POSIX Nachrichtenwarteschlange */
#define NUM_RESOURCES 3         /* Anzahl der zu überwachenden Ressourcen */

/* Array von Ressourcennamen */
const char *resources[NUM_RESOURCES] = {"cpu", "io", "memory"};

/* Struktur zum Speichern der analysierten PSI (Pressure Stall Information) Werte */
struct psi_values {
    double avg10;   /* Durchschnittlicher Druck über die letzten 10 Sekunden */
    double avg60;   /* Durchschnittlicher Druck über die letzten 60 Sekunden */
    double avg300;  /* Durchschnittlicher Druck über die letzten 300 Sekunden */
    long total;     /* Gesamtanzahl der Ereignisse */
};

/* Funktion zur Verarbeitung der PSI-Datenschnur und zum Senden an eine Nachrichtenwarteschlange */
void process_psi_data(char* data, const char* resource, int v) {
    struct psi_values psi;      /* Struktur zur Aufnahme der PSI-Daten */
    char formatted_message[BUFFER_SIZE];  /* Puffer zum Speichern der formatierten Nachricht */
    char* token;                /* Zeiger auf Token für strtok */
    const char delim[2] = " ";  /* Trennzeichen zum Tokenisieren der Datenschnur */

    //printf("Verarbeite PSI-Daten für %s: %s\n", resource, data);  /* Debug: Drucke rohe Daten */

    /* Tokenisiere die eingegebene Datenschnur */
    token = strtok(data, delim);
    while (token != NULL) {
        if (strncmp(token, "avg10=", 6) == 0) {
            psi.avg10 = atof(token + 6);  /* Wert avg10 parsen */
        } else if (strncmp(token, "avg60=", 6) == 0) {
            psi.avg60 = atof(token + 6);  /* Wert avg60 parsen */
        } else if (strncmp(token, "avg300=", 7) == 0) {
            psi.avg300 = atof(token + 7); /* Wert avg300 parsen */
        } else if (strncmp(token, "total=", 6) == 0) {
            psi.total = atol(token + 6);  /* Wert total parsen */
        }
        token = strtok(NULL, delim);     /* Fortsetzung der Tokenisierung der Datenschnur */
    }

    /* Formatiere die Nachricht, die an die Nachrichtenwarteschlange gesendet wird */
    snprintf(
        formatted_message, BUFFER_SIZE,
        "psi,resource=%s avg10=%.2f,avg60=%.2f,avg300=%.2f,total=%ld %ld\n",
        resource, psi.avg10, psi.avg60, psi.avg300, psi.total, (long int)time(NULL)
    );
    send_to_mq(formatted_message, MQ_PATH);  /* Sende die formatierte Nachricht an die Nachrichtenwarteschlange */
    if (v)
        printf("%s\n", formatted_message);
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

    int epfd = epoll_create1(0);  /* Erstelle einen epoll-Dateideskriptor */
    if (epfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    int fds[NUM_RESOURCES];  /* Dateideskriptoren für jede Ressource */
    struct epoll_event event, events[c];  /* Ereignisstruktur zur Verwendung mit epoll */
    char buf[BUFFER_SIZE];  // Puffer für das Lesen von Daten

    /* Öffne jede Ressourcendatei und richte epoll ein */
    for (int i = 0; i < NUM_RESOURCES; i++) {
        char path[256];
        snprintf(path, sizeof(path), "/proc/pressure/%s", resources[i]);
        fds[i] = open(path, O_RDONLY);
        if (fds[i] == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        memset(&event, 0, sizeof(event));
        event.events = EPOLLPRI;     /* Festlegen des Ereignistyps, auf den geprüft wird */
        event.data.fd = fds[i];      /* Festlegen des Dateideskriptors für Ereignisse */

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[i], &event) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
    }

    printf("Betritt die Hauptschleife\n");

    if (init_mq(MQ_PATH) == -1) /*mq für die bridge erstellen*/
                return 1;

    /* Hauptschleife */
    while (1) {
        int n = epoll_wait(epfd, events, c, -1);  /* Warte auf Ereignisse */
        if (n == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            lseek(fd, 0, SEEK_SET);
            ssize_t count = read(fd, buf, BUFFER_SIZE - 1);
            if (count == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (count > 0) {
                buf[count] = '\0';
                for (int j = 0; j < NUM_RESOURCES; j++) {
                    if (fd == fds[j]) {
                        process_psi_data(buf, resources[j], v);
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < NUM_RESOURCES; i++) {
        close(fds[i]);  /* Schließe alle Dateideskriptoren */
    }
    return 0;
}
