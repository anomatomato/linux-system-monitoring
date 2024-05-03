#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../include/common.h"

int line_count(FILE* file) {
    int lines = 0;

    while (!feof(file)) {
        int ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
    }

    rewind(file);
    return lines;
}
