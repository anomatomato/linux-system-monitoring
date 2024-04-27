#ifndef _CFR_NET_H_
#define _CFR_NET_H_

#include<stdio.h>

typedef struct net
{
    char* name;
    int data[16];
    struct net* next;
} net_t;

int* insert_element(FILE *file);
void count_total();
void write_file(FILE* file);
int line_count(FILE *file);

void stat();

#endif
