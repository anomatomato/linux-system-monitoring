#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "../../utilities/mq.h"

Queue* queue_head = NULL;

int line_count(FILE* file) {
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
                sprintf(placeholder, "%lld", get_timestamp());
                strcat(node->message, placeholder);
                node->next = NULL;

                if (queue_head == NULL) {
                        queue_head = node;
                        return 0;
                }

                Queue* current = queue_head;
                while (current->next != NULL) {
                        current = current->next;
                }
                current->next = node;

        } else {
                perror("malloc");
                return 1;
        }
        return 0;
}

void dequeue() {
        if (queue_head == NULL)
                return;

        Queue* current;
        while (queue_head->next != NULL) {
                current = queue_head;
                queue_head = queue_head->next;

                printf("%s\n", current->message);
                if (send_to_mq(current->message, "/cfr") == -1)
                        perror("send_to_mq failed");

                free(current);
        }
        current = queue_head;

        printf("%s", current->message);
        if (send_to_mq(current->message, "/cfr") == -1)
                perror("send_to_mq failed");

        free(current);
}
