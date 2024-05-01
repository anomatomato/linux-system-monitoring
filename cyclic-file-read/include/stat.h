#ifndef _CFR_STAT_H_
#define _CFR_STAT_H_

#include<stdio.h>

#define FILE_IN "/proc/stat"
#define FILE_OUT "stat_perc"

typedef struct cpu
{
    int name;
    float stats[10];
    struct cpu* next;
} cpu_t;

void create_list(FILE *file, int lines);
void percentages(int lines);
void write_file(FILE* file_i, FILE* file_o, int lines, int btime);
void free_list();
void stat(int btime);

#endif
