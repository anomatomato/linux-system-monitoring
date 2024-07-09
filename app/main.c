#include "bridge.h"
#include "inotify-coredump.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


void fill_memory(){
        int i = 0;
        int *array[100];
        while (i < 100) {
                int *memory = malloc(sizeof(int));
                array[i] = memory;
                i++;
        }
        for (int i = 0; i++; i < 100) {
                free(array[i]);
        }
}

int main() {
        fill_memory();
}
