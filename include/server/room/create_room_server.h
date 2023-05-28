#ifndef CREATE_ROOM_SERVER_H
#define CREATE_ROOM_SERVER_H

#include "../../../include/list/room.h"

/**
 * Detect if the message corresponding to a create room message (Start with /createroom)
 *
 * @param message The string to check
 *
 * @return 1 if the message is a create room message | 0 if the message is not a create room message
 */
extern int is_create_room_message(char* message);

/**
 * Handle a create room message
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who sent the message
 * @param pseudoClient The pseudo of the client who sent the message
 * 
 * @return void
 */
extern void handle_create_room_message(char* message, int socketClient, char* pseudoClient);

/**
 * Detect if the message is in the good format (/createroom <room_name> <room_description>)
 *
 * @param message The message to check
 *
 * @return 1 if the message is in the good format | 0 if the message is not in the good format
 */
extern int is_good_format_create_room(char* message);

/**
 * Get the room_name of the message (The second token)
 *
 * @param message The message to get the room name
 *
 * @return The room name of the message
 */
extern char* get_name_create_room(char* message);

/**
 * Get the room_description of the message (The third token)
 *
 * @param message The message to get the room description
 *
 * @return The room description of the message
 */
extern char* get_description_create_room(char* message);

#endif // CREATE_ROOM_SERVER_H