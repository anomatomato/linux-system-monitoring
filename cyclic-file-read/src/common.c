#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "../../utilities/mq.h"

Queue* queue_head = NULL;

int line_count(FILE* file) {                                                               /*zeilen einer datei zählen*/
        int lines = 0;

        while (!feof(file)) {
                int ch = fgetc(file);
                if (ch == '\n') {
                        lines++;
                }
        }
        rewind(file);

    return lines;
}

int enqueue(char* message) {
        Queue* node = (Queue *) malloc(sizeof(Queue));

        if (node) {
                char placeholder[MAX_BUFFER];

                strcpy(node->message, message);
                sprintf(placeholder, "%ld", get_timestamp());                                   /*timestamp hinzufügen*/
                strcat(node->message, placeholder);
                node->next = NULL;

                if (queue_head == NULL) {
                        queue_head = node;
                        return 0;
                }

                Queue* current = queue_head;
                while (current->next != NULL) {                                                     /*queue durchgehen*/
                        current = current->next;
                }
                current->next = node;

        } else {                                                             /*falls es kein freier speicher mehr gibt*/
                perror("malloc");
                return 1;
        }
        return 0;
}

void dequeue() {
        if (queue_head == NULL)
                return;

        Queue* current;
        while (queue_head->next != NULL) {                                      /*queue von oben nach unten durchgehen*/
                current = queue_head;
                queue_head = queue_head->next;

                //printf("%s\n", current->message);
                if (send_to_mq(current->message, "/cfr") == -1)                                     /*an die mq senden*/
                        perror("send_to_mq failed");

                free(current);                                                                /*alloziiertes freigeben*/
        }
        current = queue_head;

        //printf("%s\n", current->message);
        if (send_to_mq(current->message, "/cfr") == -1)
                perror("send_to_mq failed");

        free(current);
        queue_head = NULL;                                                     /*head auf NULL für den nächsten zyklus*/
}

void empty_queue() {                                                                            /*queue leeren für sys*/
        queue_head = NULL;
}
