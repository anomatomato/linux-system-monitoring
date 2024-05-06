#include "get_pacct.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/acct.h>
#include <sys/stat.h>
#include <unistd.h>

// server path
// const char* binary_pacct = "/var/log/pacct/binary_pacct";
// const char* original_pacct  =
// "/opt/stats-recording/inotify-pacct/src/original_pacct.txt"; const char*
// copy_pacct = "/opt/stats-recording/inotify-pacct/src/copy_pacct.txt"; const
// char* diff = "/opt/stats-recording/inotify-pacct/src/diff.txt";

// local path
const char* binary_pacct = "/var/log/pacct/binary_pacct";
const char* original_pacct =
    "/var/log/pacct/original_pacct.txt";
const char* copy_pacct =
    "/var/log/pacct/copy_pacct.txt";
const char* diff = "/var/log/pacct/diff.txt";

/**
 * Copies a file from the source path to the destination path.
 *
 * @param source The path of the source file.
 * @param destination The path of the destination file.
 */
void copy(const char* source, const char* destination)
{
    printf("%s", source);
    FILE* src = fopen(source, "r");
    FILE* dst = fopen(destination, "w");

    if (!src || !dst)
    {
        printf("%p\n", src); // If src is a FILE*
        printf("%p\n", dst); // If dst is a FILE*
        fprintf(stderr, "Error opening file\n");
        if (src)
            fclose(src);
        if (dst)
            fclose(dst);
        exit(1);
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);
}

/**
 * Writes the differences between two files to a new file.
 *
 * @param old_file The path of the old file.
 * @param new_file The path of the new file.
 * @param diff_file The path of the file to write the differences to.
 */
void write_diff(const char* old_file, const char* new_file,
                const char* diff_file)
{
    FILE* old  = fopen(old_file, "r");
    FILE* newf = fopen(new_file, "r");
    FILE* diff = fopen(diff_file, "w");

    if (!old || !newf || !diff)
    {
        fprintf(stderr, "Error opening file\n");
        if (old)
            fclose(old);
        if (newf)
            fclose(newf);
        if (diff)
            fclose(diff);
        exit(1);
    }
    char line1[4096];
    char line2[4096];
    while (fgets(line2, sizeof(line2), newf))
    {
        fgets(line1, sizeof(line1), old);
        if (feof(old))
        {
            fprintf(diff, "%s", line2);
        }
    }

    fclose(old);
    fclose(newf);
    fclose(diff);
}

/**
 * Enables process accounting, saves records to a specified file,
 * sleeps for 10 seconds, and then disables process accounting.
 *
 * @return EXIT_SUCCESS if process accounting is enabled and disabled
 * successfully, EXIT_FAILURE otherwise.
 */
int enable_process_accounting()
{
    // Attempt to enable process accounting
    if (acct(binary_pacct) == -1)
    {
        perror("Failed to enable process accounting");
        return EXIT_FAILURE;
    }

    printf("Process accounting enabled. Records are being saved to: %s\n",
           binary_pacct);

    sleep(10);

    // Disable process accounting
    if (acct(NULL) == -1)
    {
        perror("Failed to disable process accounting");
        return EXIT_FAILURE;
    }

    printf("Process accounting disabled.\n");

    return EXIT_SUCCESS;
}

/**
 * Converts binary accounting data to a human-readable text file.
 * The function uses the `dump-acct` command to convert the binary accounting
 * data stored in `binary_acct` to a human-readable format and writes it to the
 * specified text file located at
 * `/opt/stats-recording/inotify-pacct/src/pacct.txt`.
 *
 * @return Returns `EXIT_SUCCESS` if the conversion and writing process is
 * successful, otherwise returns `EXIT_FAILURE`.
 */
int convert_binary_to_text()
{
    char command[256];
    snprintf(command, sizeof(command), "dump-acct %s > %s", binary_pacct,
             original_pacct);

    if (system(command) == -1)
    {
        perror("Failed to execute command");
        return EXIT_FAILURE;
    }

    printf(
        "All process accounting datas have been translated in human-readable "
        "format and written to: %s\n",
        original_pacct);

    return EXIT_SUCCESS;
}

/**
 * @brief Gets continuously the new process accounting datas.
 *
 * This function runs in an infinite loop. Fistly it enables process accounting
 * and converts this to text format. Then the differences between the current
 * and previous version of the text file are found, and written to a diff file.
 *
 * @return int Returns EXIT_SUCCESS if the process accounting is successful,
 * otherwise returns EXIT_FAILURE.
 */
int get_pacct()
{
    if (enable_process_accounting() == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in process accounting. Exiting.\n");
        return EXIT_FAILURE;
    }

    if (convert_binary_to_text() == EXIT_FAILURE)
    {
        fprintf(stderr,
                "Error converting process accounting data to text. Exiting.\n");
        return EXIT_FAILURE;
    }

    // Initial copy of the file
    copy(original_pacct, copy_pacct);
    copy(original_pacct, diff);
    while (1)
    {
        if (enable_process_accounting() == EXIT_FAILURE)
        {
            fprintf(stderr, "Error in process accounting. Exiting.\n");
            return EXIT_FAILURE;
        }

        if (convert_binary_to_text() == EXIT_FAILURE)
        {
            fprintf(
                stderr,
                "Error converting process accounting data to text. Exiting.\n");
            return EXIT_FAILURE;
        }

        // Write the differences
        write_diff(copy_pacct, original_pacct, diff);

        // Update the copy file with the new content
        copy(original_pacct, copy_pacct);

        sleep(10);
    }
    return EXIT_SUCCESS;
}