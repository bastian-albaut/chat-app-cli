#ifndef PRIVATE_MESSAGE_SERVER_H
#define PRIVATE_MESSAGE_SERVER_H

#include "../../list/list.h"

/**
 * Detect if the message corresponding to a private message (start with "/mp")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a private message | 0 if the message is not a private message
 */
extern int is_private_message(char* message);

/**
 * Send private message to a client. OR send an error message if the private message
 * is not in the correct format or if the pseudo of the client does not exist. OR 
 * do nothing if the message is not a private message
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 *
 * @return void
 */
extern void handle_private_message(char* message, int socketClient, char* pseudoTransmitter);

/**
 * Detect if the message corresponding to the format /mp <user> <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_private_message(char* message);

/**
 * Get the destination pseudo of the private message
 *
 * @param message The string to split to get the pseudo
 *
 * @return The pseudo of the private message
 */
extern char* get_pseudo_private_message(char* message);

/**
 * Get content of the private message
 *
 * @param message The string to split to get the content
 *
 * @return The content of the private message
 */
extern char* get_content_private_message(char* message);

/**
 * Send private message to the client with the pseudo specified in parameter
 *
 * @param head The head of the list of clients
 * @param pseudo The pseudo of the client to send the message
 * @param message The message to send
 *
 * @return 1 if the message is send | -1 if the message is not send
 */
extern int send_private_message(Node* head, char* pseudo, char* message, char* pseudoTransmitter);

#endif // PRIVATE_MESSAGE_SERVER_H