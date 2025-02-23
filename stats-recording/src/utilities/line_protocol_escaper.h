#include <stddef.h>

/**
 * @brief Escape special characters in tag values.
 * Special characters are backslash, whitespace, comma
 *
 * @param tag_value: The tag value after '='
 * @param output: The buffer, that the escaped tag value should be written to
 * @param output_size: Maximum size of bytes written to output
 *     @retval  >0  Size of bytes written to output on success
 *     @retval  -1  failure
 */
size_t escape_tag_value(const char *tag_value, char *output, size_t output_size);

/**
 * @brief Escape special characters in field values.
 * Special characters are backslash and ".
 *
 * @param tag_value: The field value after '='
 * @param output: The buffer, that the escaped field value should be written to
 * @param output_size: Maximum size of bytes written to output
 *     @retval  >0  Size of bytes written to output on success
 *     @retval  -1  failure
 */
size_t escape_field_value(const char *field_value, char *output, size_t output_size);