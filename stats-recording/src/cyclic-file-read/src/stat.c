#include "stat.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Cpu_t *stat_head = NULL;

int create_list(FILE *file, int lines) {
        for (int i = 0; i < (lines - 7); i++) {                /*lines-7 ist anzahl der kerne + total*/
                Cpu_t *node = (Cpu_t *) malloc(sizeof(Cpu_t)); /*dynamisch allozieren pro kern*/

                if (node) {
                        char line_buffer[MAX_LINE];
                        fgets(line_buffer, MAX_LINE, file); /*dateizeile lesen*/

                        if (line_buffer[strlen(line_buffer) - 1] != '\n') {
                                printf("File too big.\n"); /*falls die zeile zu groß ist*/
                                return 1;
                        }

                        char *token;
                        token = strtok(line_buffer, " "); /*zeile unterteilen*/

                        float f;

                        node->name = i;

                        for (int ii = 0; ii < 10;
                             ii++) { /*werte ermitteln und den jeweiligen kernen anordnen*/
                                token = strtok(NULL, " ");
                                sscanf(token, "%f", &f); /*strings zu float für die prozent ermittlung*/
                                node->stats[ii] = f;
                        }

                        node->next = NULL;

                        if (stat_head == NULL) { /*listen verkettung*/
                                stat_head = node;
                        } else {
                                Cpu_t *current = stat_head;
                                while (current->next != NULL) {
                                        current = current->next;
                                }
                                current->next = node;
                        }

                } else { /*falls kein speicher mehr übrig ist*/
                        perror("malloc");
                        return 1;
                }
        }
        return 0;
}

void percentages(int lines) {
        //float totals[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; /*array um total-werte zu speichern*/

        Cpu_t *current;
        //for (int i = 0; i < 10; i++) { /*teilwerte zusammenaddieren*/
                //current = stat_head;
                //for (int j = 0; j < (lines - 7); j++) {
                        //totals[i] += current->stats[i];
                        //current = current->next;
                //}
        //}

        current = stat_head;
        current = current->next;
        for (int i = 1; i < (lines - 7); i++) { /*prozent-anteile berechnen*/
                for (int j = 0; j < 10; j++) {
                        if (stat_head->stats[j] != 0) {
                                current->stats[j] = (current->stats[j]) / stat_head->stats[j];
                                current->stats[j] = current->stats[j] * 100;
                        } else {
                                current->stats[j] = 0;
                        }
                }
                current = current->next;
        }
}

int write_stat_message(FILE *file, int lines) {
        char line_buffer[MAX_LINE];
        fscanf(file, "%*[^\n]"); /*intr überspringen*/
        fgets(line_buffer, MAX_LINE, file);
        fgets(line_buffer, MAX_LINE, file);

        if (line_buffer[strlen(line_buffer) - 1] != '\n') {
                printf("File too big.\n"); /*falls zeile zu groß ist*/
                return 1;
        }

        char *token;
        token = strtok(line_buffer, " ");
        token = strtok(NULL, " "); /*zeile unterteilen*/
        token[strlen(token) - 1] = '\0';

        extern char stat_form[13][MAX_BUFFER]; /*fürs line protokol*/

        char message[MAX_LINE];
        message[0] = '\0';

        char placeholder[MAX_BUFFER]; /*für float zu string umwandlung*/
        placeholder[0] = '\0';

        Cpu_t *current = stat_head;

        for (int i = 0; i < (lines - 7); i++) {
                strcat(message, stat_form[0]);
                if (i != 0) { /*ersten eintrag ausschließen*/
                        current->name -= 1;
                        sprintf(placeholder, "%d", current->name);
                        strcat(message, placeholder);
                }
                for (int ii = 0; ii < 10; ii++) { /*werte ins format bringen*/
                        strcat(message, stat_form[ii + 1]);
                        sprintf(placeholder, "%.2f", current->stats[ii]);
                        strcat(message, placeholder);
                }
                strcat(message, stat_form[11]);
                strcat(message, token); /*ctext hinzu*/
                strcat(message, stat_form[12]);

                if (enqueue(message) == 1) /*in die queue anreihen*/
                        return 1;

                message[0] = '\0';       /*message leeren und wiederverwenden*/
                current = current->next; /*nächster kern*/
        }
        return 0;
}

void free_list() {
        Cpu_t *current;

        while (stat_head != NULL) { /*alles alloziierte befreien*/
                current = stat_head;
                stat_head = stat_head->next;
                free(current);
        }
}

int stat() {
        FILE *statf;
        int lines;

        if ((statf = fopen(STAT_FILE, "r")) == NULL) { /*datei der werte öffnen*/
                perror("fopen");
                return 1;
        }

        lines = line_count(statf); /*kernanzahl anhand von zeilenanzahl bestimmen*/

        if (create_list(statf, lines) == 1) /*werte der kerne in einer liste zusammentragen*/
                return 1;

        percentages(lines);                        /*prozentuale anteile berechnen*/
        if (write_stat_message(statf, lines) == 1) /*fürs line protokol anreihen*/
                return 1;

        free_list();   /*alloziiertes befreien*/
        fclose(statf); /*datei schließen*/
        return 0;
}
