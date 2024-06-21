#ifndef _CFR_COMM_H_
#define _CFR_COMM_H_

#include <stdio.h>

#define MAX_LINE 1024                                                                 /*platz für das lesen von zeilen*/
#define MAX_BUFFER 64                                                       /*hauptsächlich platz für die form-strings*/

typedef struct CFRqueue {                                                        /*queue zum sammeln der line messages*/
        char message[MAX_LINE];
        struct CFRqueue* next;
} Queue;

int line_count(FILE* file);

int enqueue(char* message);

void dequeue();

void empty_queue();

#endif
