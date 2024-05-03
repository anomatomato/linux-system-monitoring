#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/stat.h"

cpu_t* stat_head = NULL;

void create_list(FILE* file, int lines) {
    for (int i = 0; i < (lines-7); i++) {
        cpu_t *node = (cpu_t *) malloc(sizeof(cpu_t));

        if (node) {
            char buffer[128];
            fgets(buffer, 128, file);
            char* token;
            float f;

            if (i == 0) {
                token = strtok(buffer, "  ");
            } else {
                token = strtok(buffer, " ");
            }

            node->name = i;

            for (int ii = 0; ii < 10; ii++) {
                token = strtok(NULL, " ");
                sscanf(token, "%f", &f);
                node->stats[ii] = f;
            }

            node->next = NULL;

            if (stat_head == NULL) {
                stat_head = node;
            } else {
                cpu_t *current = stat_head;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = node;
            }

        } else {
            perror("No available Memory");
            exit(EXIT_FAILURE);
        }
    }
}

void percentages(int lines) {
    float totals[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    cpu_t* current;
    for (int i = 0; i < 10; i++) {
        current = stat_head;
        for (int j = 0; j < (lines - 7); j++) {
            totals[i] += current->stats[i];
            current = current->next;
        }
    }
    current = stat_head;
    for (int i = 0; i < (lines-7); i++) {
        for (int j = 0; j < 10; j++) {
            if (totals[j] != 0) {
                current->stats[j] = (current->stats[j])/totals[j];
                current->stats[j] = current->stats[j] * 100;
            } else {
                current->stats[j] = 0;
            }
        }
        current = current->next;
    }
}

void write_file(FILE* file_i, FILE* file_o, int lines, int btime) {

    char buffer[128];
    fscanf(file_i, "%*[^\n]");
    fgets(buffer, 128, file_i);

    fgets(buffer, 128, file_i);
    char* token;
    token = strtok(buffer, " ");
    token = strtok(NULL, " ");
    int ctext;
    sscanf(token, "%d", &ctext);

    /*fgets(buffer, 128, file_i);
    token = strtok(buffer, " ");
    token = strtok(NULL, " ");
    int btime;
    sscanf(token, "%d", &btime);*/

    cpu_t* current = stat_head;
    for (int i = 0; i < (lines-7); i++) {
        if (current->name == 0) {
            fprintf(file_o, "proc-stat,core=cpu user=%f,nice=%f,system=%f,idle=%f,iowait=%f,irq=%f,softirq=%f,steal=%f,guest=%f,guest_nice=%f,ctext=%d %d\n", current->stats[0], current->stats[1], current->stats[2], current->stats[3], current->stats[4], current->stats[5], current->stats[6], current->stats[7], current->stats[8], current->stats[9], ctext, btime);
        } else {
            fprintf(file_o, "proc-stat,core=cpu%d user=%f,nice=%f,system=%f,idle=%f,iowait=%f,irq=%f,softirq=%f,steal=%f,guest=%f,guest_nice=%f,ctext=%d %d\n", (current->name - 1), current->stats[0], current->stats[1], current->stats[2], current->stats[3], current->stats[4], current->stats[5], current->stats[6], current->stats[7], current->stats[8], current->stats[9], ctext, btime);
        }
        current = current->next;
    }
}

void free_list() {
    cpu_t* current;

    while (stat_head != NULL)
    {
        current = stat_head;
        stat_head = stat_head->next;
        free(current);
    }
}

void stat(int lines, char* btime) {

    FILE* stat;

    if ((stat = fopen(FILE_IN, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    create_list(stat, lines);
    percentages(lines);
    //write_file(stat, stat_perc, lines, btime);
    free_list();

    fclose(stat);
}
