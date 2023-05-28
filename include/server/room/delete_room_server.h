#ifndef DELETE_ROOM_SERVER_H
#define DELETE_ROOM_SERVER_H

#include "../../../include/list/room.h"

/**
 * Detect if the message corresponding to a delete room message (Start with /deleteroom)
 *
 * @param message The string to check
 *
 * @return 1 if the message is a delete room message | 0 if the message is not a delete room message
 */
extern int is_delete_room_message(char* message);

/**
 * Handle a delete room message
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who sent the message
 * @param pseudoClient The pseudo of the client who sent the message
 * 
 * @return void
 */
extern void handle_delete_room_message(char* message, int socketClient, char* pseudoClient);

/**
 * Detect if the message is in the good format (/deleteroom <room_name>)
 *
 * @param message The message to check
 *
 * @return 1 if the message is in the good format | 0 if the message is not in the good format
 */
extern int is_good_format_delete_room(char* message);

/**
 * Get the room_name of the message (The second token)
 *
 * @param message The message to get the room name
 *
 * @return The room name of the message
 */
extern char* get_name_delete_room(char* message);

#endif // DELETE_ROOM_SERVER_H