#ifndef _MQ_H_
#define _MQ_H_

/** @file mq.h
 *
 *  @brief Helper Macros and functions for message queues
 */

#define MAX_MSG_SIZE 1024
#define MAX_MESSAGES 10

#include <mqueue.h>

/**
 * @brief init_mq initializes a message queue with the given name and returns
 * the file descriptor or failure
 *
 * @param mq_path: The identifying name of the message queue
 *     @retval  File descriptor of the new message queue on success
 *     @retval  -1               failure, with errno set appropriately
 */
int init_mq(const char *mq_path);

/**
 * @brief send_to_mq sends the given message to the given message queue and
 * returns success or failure
 *
 * @param message: The message to send
 * @param mq_path: The identifying name of the message queue
 *     @retval  0   success
 *     @retval -1  on failure with errno set appropriately
 */
int send_to_mq(const char *message, const char *mq_path);

/**
 * @brief remove_mq immediately removes the specified message queue
 *
 * @param mq_path: The identifying name of the message queue
 */
void remove_mq(const char *mq_path);

/**
 * @brief get_timestamp returns the current time in nanoseconds
 *
 * @return The current time in nanoseconds
 */
long long get_timestamp();

#endif
