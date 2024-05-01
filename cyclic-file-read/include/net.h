#ifndef _CFR_NET_H_
#define _CFR_NET_H_

#include<stdio.h>

typedef struct net
{
    char* name;
    int data[16];
    struct net* next;
} net_t;

#endif
