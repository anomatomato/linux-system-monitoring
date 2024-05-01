#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/net.h"

net_t* net_head = NULL;

int* insert_element(FILE* file) {
    char buffer[128];
    fgets(buffer, 128, file);
    fgets(buffer, 128, file);

    net_t* node = (net_t*)malloc(sizeof(net_t));

    if (node) {
        fgets(buffer, 128, file);
        printf("%s\n", buffer);
        char* token = strtok(buffer, " ");
        float f;

        node->name = token;

        for (int i = 0; i < 10; i++) {
            token = strtok(NULL, " ");
            sscanf(token, "%f", &f);
            node->data[i] = f;
        }

        node->next = NULL;

        if (net_head == NULL) {
            net_head = node;
            return 0;
        }

        net_t* current = net_head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
        return 0;
    }
    perror("No available Memory");
    return NULL;

}

void write_file(FILE* file) {

}

void dev() {
    FILE* net;
    FILE* net_out;

    if ((net = fopen("/proc/dev/net", "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if ((net_out = fopen("net_out", "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int lines = line_count(net);
    printf("lines:%d\n", lines);

    for (int i = 0; i < lines; i++) {
        insert_element(net);
    }

    write_file(net_out);

    fclose(net);
    fclose(net_out);
    free(net_head);
}
