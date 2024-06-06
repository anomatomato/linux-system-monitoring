#ifndef _PACCT_TO_LINE_PROTOCOL_H_
#define _PACCT_TO_LINE_PROTOCOL_H_

#include "pacct.h"
#include <sys/acct.h>   // For process accounting definitions
#include <time.h>       // For time_t, localtime, strftime, struct tm
#include <stdint.h>     // For fixed-width integer types
#include <string.h>     // For string manipulation functions
#include "mq.h"         // For get_timestamp()

double comp_to_double(comp_t comp);
double time_comp_to_double(comp_t comp);
char* format_time(time_t start_time);
const char* get_flag_string(char ac_flag);
void construct_payload(struct acct_v3 *acct_record, char *payload, size_t payload_size);

#endif
