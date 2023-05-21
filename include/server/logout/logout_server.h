#ifndef LOGOUT_SERVER_H
#define LOGOUT_SERVER_H

#include <pthread.h>

/**
 * Detect if the message corresponding to a logout message (start with "/logout")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a logout message | 0 if the message is not a logout message
 */
extern int is_logout_message(char* message);

/**
 * Handle logout message. Send a message to the client to confirm the logout.
 *
 * @param message The message check
 * @param socketClient The socket of the client who want to logout
 *
 * @return void
 */
extern void handle_logout_message(char* message, int socketClient, pthread_t threadId);

/**
 * Detect if the message corresponding to the format /logout
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_logout_message(char* message);

#endif // LOGOUT_SERVER_H