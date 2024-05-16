#include <sys/acct.h>
#include <stdio.h>
#include <stdlib.h>

#define BINARY "/var/log/pacct/binary"
#define PACCT "/var/log/pacct/pacct"

int convert_binary_to_text()
{
    char command[256];
    FILE* file = fopen(BINARY, "w");
    if (!file)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    snprintf(command, sizeof(command), "dump-acct %s > %s", BINARY, PACCT);

    if (system(command) == -1)
    {
        perror("Failed to execute command");
        return EXIT_FAILURE;
    }

    printf(
        "All process accounting datas have been translated in human-readable "
        "format and written to: %s\n",
        PACCT);
    fclose(file);
    return EXIT_SUCCESS;
}

int main()
{
    FILE* file = fopen(BINARY, "w");

    // Attempt to enable process accounting
    if (acct(BINARY) == -1)
    {
        perror("Failed to enable process accounting");
        return EXIT_FAILURE;
    }

    printf("Process accounting enabled. Records are being saved to: %s\n",
           BINARY);


    if (convert_binary_to_text() != EXIT_SUCCESS)
    {
        perror("Failed to convert binary to text");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}