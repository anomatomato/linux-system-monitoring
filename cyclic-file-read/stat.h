#ifndef _CFR_STAT_H_
#define _CFR_STAT_H_

#include<stdio.h>

typedef struct cpu
{
    char* name;
    float stats[10];
    struct cpu* next;
} cpu_t;

int* insert_element(FILE *file);
void count_total();
void write_file(FILE* file);
int line_count(FILE *file);

void stat();

#endif
