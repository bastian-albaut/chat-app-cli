#ifndef MSG_ROOM_SERVER_H
#define MSG_ROOM_SERVER_H

#include "../../../include/list/room.h"

/**
 * Detect if the message corresponding to a msg room message (Start with /msgroom)
 *
 * @param message The string to check
 *
 * @return 1 if the message is a msg room message | 0 if the message is not a msg room message
 */
extern int is_msg_room_message(char* message);

/**
 * Handle a msg room message
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who sent the message
 * @param pseudoClient The pseudo of the client who sent the message
 * 
 * @return void
 */
extern void handle_msg_room_message(char* message, int socketClient, char* pseudoClient);

/**
 * Detect if the message is in the good format (/msgroom <message>)
 *
 * @param message The message to check
 *
 * @return 1 if the message is in the good format | 0 if the message is not in the good format
 */
extern int is_good_format_msg_room(char* message);

/**
 * Send message to other clients in the room whithout client corresponding to socketClient
 *
 * @param message The message to send
 * @param room The room where the message is send
 * @param socketClient The socket of the client who send the message
 * @param pseudoTransmitter The pseudo of the client who send the message
 *
 * @return 1 if all messages are send | -1 if at least one message is not send | 0 if there is no other clients in the room
 */
extern int send_to_other_clients_room(char* message, Room* room, int socketClient, char* pseudoTransmitter);

/**
 * Get the content of the message (without /msgroom)
 *
 * @param message The message to get the content
 *
 * @return The content of the message
 */
extern char* get_content_msg_room(char* message);

#endif // MSG_ROOM_SERVER_H