#include <stdio.h>
#include <stdlib.h>

int main() {
    int *p = NULL;
    *p = 42;  // This line causes a segmentation fault
    return 0;
}
