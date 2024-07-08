#include "line_protocol_escaper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


size_t escape_tag_value(const char *tag_value, char *output, size_t output_size) {
        if (tag_value == NULL || output == NULL || output_size == 0)
                return -1;

        size_t i, j;
        for (i = 0, j = 0; tag_value[i] != '\0' && j < output_size - 1; i++) {
                if (tag_value[i] == ' ' || tag_value[i] == '=' || tag_value == ',')
                        output[j++] = '\\';

                output[j++] = tag_value[i];
        }
        output[j] = '\0';
        return j;
}

size_t escape_field_value(const char *field_value, char *output, size_t output_size) {
        if (field_value == NULL || output == NULL || output_size == 0)
                return -1;

        size_t i, j;
        for (i = 0, j = 0; field_value[i] != '\0' && j < output_size - 1; i++) {
                if (field_value[i] == '\\' || field_value[i] == '"')
                        output[j++] = '\\';

                output[j++] = field_value[i];
        }
        output[j] = '\0';
        return j;
}