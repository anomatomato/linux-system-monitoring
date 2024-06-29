#include <stdio.h>
#include <stdlib.h>

void write_string_to_file(const char *filename, const char *content) {
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
                perror("Failed to open file for writing");
                exit(EXIT_FAILURE);
        }

        if (fputs(content, file) == EOF) {
                perror("Failed to write to file");
                fclose(file);
                exit(EXIT_FAILURE);
        }

        fclose(file);
}

void read_file_into_buffer(const char *filename, char *buffer, size_t buffer_size) {
        FILE *file = fopen(filename, "r");
        if (!file) {
                perror("Failed to open file");
                exit(EXIT_FAILURE);
        }

        size_t bytesRead = fread(buffer, 1, buffer_size, file);
        if (bytesRead == 0 && ferror(file)) {
                perror("Failed to read from file");
                fclose(file);
                exit(EXIT_FAILURE);
        }

        buffer[bytesRead] = '\0'; // Null-terminate the buffer

        fclose(file);
}