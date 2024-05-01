#include "dev.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

net_t* head     = NULL;
int line_number = 0;

int* insert_element(FILE* file)
{
    char buffer[128];
    fgets(buffer, 128, file);
    fgets(buffer, 128, file);

    net_t* node = (net_t*)malloc(sizeof(net_t));

    if (node)
    {
        fgets(buffer, 128, file);
        printf("%s\n", buffer);
        char* token = strtok(buffer, " ");
        float f;

        node->name = token;

        for (int i = 0; i < 10; i++)
        {
            token = strtok(NULL, " ");
            sscanf(token, "%f", &f);
            node->stats[i] = f;
        }

        node->next = NULL;

        if (head == NULL)
        {
            head = node;
            return 0;
        }

        net_t* current = head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = node;
        return 0;
    }
    perror("No available Memory");
    return NULL;
}

void write_file(FILE* file)
{
}

int line_count(FILE* file)
{
    int lines = 0;

    while (!feof(file))
    {
        int ch = fgetc(file);
        if (ch == '\n')
        {
            lines++;
        }
    }

    rewind(file);
    return lines;
}

void dev()
{
    FILE* net;
    FILE* net_out;

    if ((net = fopen("/proc/dev/net", "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if ((net_out = fopen("net_out", "w")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    line_number = line_count(net);
    printf("lines:%d\n", line_number);

    for (int i = 0; i < line_number; i++)
    {
        insert_element(net);
    }

    count_total();

    write_file(net_out);

    fclose(net);
    fclose(net_out);
    free(head);
}

int main()
{
    dev();
    return 0;
}
