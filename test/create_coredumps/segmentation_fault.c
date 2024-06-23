#include <stdio.h>

int main()
{
    int* ptr = NULL;
    printf("%d\n", *ptr); // This will create a segmentation fault
    return 0;
}
