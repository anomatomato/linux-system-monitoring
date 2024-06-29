#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>

int read_file_into_buffer(const char *filename, char *buffer, size_t buffer_size) {
        FILE *file = fopen(filename, "r");
        if (!file) {
                perror("Failed to open file");
                return -1;
        }

        size_t bytesRead = fread(buffer, 1, buffer_size, file);
        if (bytesRead == 0 && ferror(file)) {
                perror("Failed to read from file");
                fclose(file);
                return -1;
        }

        buffer[bytesRead] = '\0'; /* Null-terminate the buffer */

        fclose(file);
        return bytesRead;
}