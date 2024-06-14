#ifndef _PACCT_TO_LINE_PROTOCOL_H_
#define _PACCT_TO_LINE_PROTOCOL_H_

#include "mq.h" // For get_timestamp()
#include "pacct.h"
#include <stdint.h>   // For fixed-width integer types
#include <string.h>   // For string manipulation functions
#include <sys/acct.h> // For process accounting definitions
#include <time.h>     // For time_t, localtime, strftime, struct tm

/**
 * Converts a comp_t value to a double.
 *
 * @param comp The comp_t value to convert.
 * @return The converted double value.
 */
double comp_to_double(comp_t comp);

/**
 * Converts a comp_t value representing time to a double value.
 *
 * @param comp The comp_t value to convert.
 * @return The converted time value as a double.
 */
double time_comp_to_double(comp_t comp);

/**
 * Formats a given time value into a string representation.
 *
 * @param start_time The time value to be formatted.
 * @return A pointer to a string containing the formatted time.
 */
char* format_time(time_t start_time);

/**
 * Returns a string representation of the given flag.
 *
 * @param ac_flag The flag to convert to a string.
 * @return The string representation of the flag.
 */
const char* get_flag_string(char ac_flag);

/**
 * Constructs a payload string from the given `acct_record` in line protocol
 * and stores it in the `payload` buffer.
 *
 * @param acct_record The `acct_v3` structure containing the accounting record.
 * @param payload The buffer to store the constructed payload string.
 * @param payload_size The size of the `payload` buffer.
 */
void construct_payload(struct acct_v3* acct_record, char* payload,
                       size_t payload_size);

#endif
