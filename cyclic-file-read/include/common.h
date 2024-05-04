#ifndef _COMM_H_
#define _COMM_H_

#include<stdio.h>

#define MAX_LINE 512                                                            /*platz für das lesen von zeilen*/
#define MAX_BUFFER 64                                                   /*hauptsächlich platz für die form-strings*/

int line_count(FILE* file);

#endif
