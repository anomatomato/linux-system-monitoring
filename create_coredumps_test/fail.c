#include <stdio.h>

int main()
{
    int *ptr = NULL;
    printf("%d\n", *ptr); // Dies wird einen Segmentierungsfehler verursachen
    return 0;
}
