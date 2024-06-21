#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H

extern volatile int keep_running;

/**
 * @brief Signal handler function for gracefully shutting down the program.
 *
 * This function is set as the handler for SIGINT, SIGTERM, and other signals.
 * When a signal is received, this function sets keep_running to 0, which indicates the program should
 * stop running.
 *
 * @param signal The signal number of the received signal.
 */
void handle_signal(int signal);
int setup_signal_handling();


#endif