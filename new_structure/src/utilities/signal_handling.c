#include "signal_handling.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t keep_running = 1;

/* Only async-safe function `write` is used to print a message */
void print_signal(int signal) {
        char msg1[] = "\nReceived signal ";
        char msg2[] = ", stopping...\n";
        char num[10];
        int length = 0;

        /* Write first part of the message */
        write(STDOUT_FILENO, msg1, sizeof(msg1) - 1);

        /* Convert reversed signal number to string */
        int sig = signal;
        do {
                num[length++] = '0' + (signal % 10);
                sig /= 10;
        } while (sig > 0);

        /* Write the signal number in right order */
        for (int i = length - 1; i >= 0; i--) {
                write(STDOUT_FILENO, &num[i], 1);
        }

        /* Write the final part of the message */
        write(STDOUT_FILENO, msg2, sizeof(msg2) - 1);
}

/*------------------------------------------------------------------------------------------------------*/


/* Signal handler */
void handle_signal(int signal) {
        print_signal(signal);
        keep_running = 0;
}
