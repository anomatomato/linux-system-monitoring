#ifndef _CFR_STAT_H_
#define _CFR_STAT_H_

#include<stdio.h>

#define STAT_FILE "/proc/stat"

typedef struct Cpu {                    /*struct, da unterschiedliche kernanzahl und werte müssen bearbeitet werden*/
        int name;
        float stats[10];
        struct Cpu* next;
} Cpu_t;

void stat(char* btime);

#endif
