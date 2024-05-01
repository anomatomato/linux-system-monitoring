#include "stat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cpu_t* head     = NULL;
int line_number = 0;

int* insert_element(FILE* file)
{
    cpu_t* node = (cpu_t*)malloc(sizeof(cpu_t));

    if (node)
    {
        char buffer[128];
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

        cpu_t* current = head;
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

void count_total()
{
    float totals[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    cpu_t* current   = head;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < (line_number - 7); j++)
        {
            totals[i] += current->stats[i];
            current = current->next;
        }
    }
    current = head;
    for (int i = 0; i < (line_number - 7); i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (totals[j] != 0)
            {
                current->stats[j] = (current->stats[j]) / totals[j];
            }
            else
            {
                current->stats[j] = 0;
            }
        }
        current = current->next;
    }
}

void write_file(FILE* file)
{
    cpu_t* current = head;
    for (int i = 0; i < (line_number - 7); i++)
    {
        fprintf(file, "%s ", head->name);
        for (int j = 0; j < 10; j++)
        {
            fprintf(file, "%f ", head->stats[j]);
        }
        fprintf(file, "\n");
    }
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

void stat()
{
    FILE* stat;
    FILE* stat_perc;

    if ((stat = fopen("/proc/stat", "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if ((stat_perc = fopen("stat_perc", "w")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    line_number = line_count(stat);
    printf("lines:%d\n", line_number);

    for (int i = 0; i < (line_number - 7); i++)
    {

        insert_element(stat);
    }

    count_total();

    write_file(stat_perc);

    fclose(stat);
    fclose(stat_perc);
    free(head);
}

int main()
{
    stat();
    return 0;
}
