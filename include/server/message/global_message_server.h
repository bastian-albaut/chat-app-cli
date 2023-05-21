#ifndef GLOBAL_MESSAGE_SERVER_H
#define GLOBAL_MESSAGE_SERVER_H

/**
 * Detect if the message corresponding to a global message (start with "/all")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a global message | 0 if the message is not a global message
 */
extern int is_global_message(char* message);

/**
 * Handle global message. Send the message to all clients.
 *
 * @param head The head of the list of clients
 * @param socketClient The socket of the client who send the message
 * @param message The message to send
 *
 * @return void
 */
extern void handle_global_message(char* message, int socketClient, char* pseudoTransmitter);

/**
 * Detect if the message corresponding to the format /all <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_global_message(char* message);

/**
 * Get content of the global message
 *
 * @param message The string to split to get the content
 *
 * @return The content of the global message
 */
extern char* get_content_global_message(char* message);

/**
 * Send message to all clients whithout client corresponding to socketClient
 *
 * @param message The message to send
 * @param socketClient The socket of the client who send the message
 * @param pseudoTransmitter The pseudo of the client who send the message
 *
 * @return 1 if all messages are send | -1 if at least one message is not send | 0 if there is no other clients
 */
extern int send_to_other_clients(char* message, int socketClient, char* pseudoTransmitter);

#endif // GLOBAL_MESSAGE_SERVER_H