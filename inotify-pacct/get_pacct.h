#ifndef _GET_PACCT_H_
#define _GET_PACCT_H_

void copy(const char* source, const char* destination);
void write_diff(const char* old_file, const char* new_file,
                const char* diff_file);
int enable_process_accounting();
int convert_binary_to_text();

#endif