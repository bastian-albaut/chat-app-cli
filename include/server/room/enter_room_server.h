#ifndef ENTER_ROOM_SERVER_H
#define ENTER_ROOM_SERVER_H

/**
 * Detect if the message corresponding to a join room message (Start with /joinroom)
 *
 * @param message The string to check
 *
 * @return 1 if the message is a join room message | 0 if the message is not a join room message
 */
extern int is_join_room_message(char* message);

/**
 * Handle a join room message
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who sent the message
 * @param pseudoClient The pseudo of the client who sent the message
 * 
 * @return void
 */
extern void handle_join_room_message(char* message, int socketClient, char* pseudoClient);

/**
 * Detect if the message is in the good format (/joinroom <room_name>)
 *
 * @param message The message to check
 *
 * @return 1 if the message is in the good format | 0 if the message is not in the good format
 */
extern int is_good_format_join_room(char* message);

/**
 * Get the room name from a join room message
 *
 * @param message The message to check
 *
 * @return The room name
 */
extern char* get_room_name(char* message);

#endif // ENTER_ROOM_SERVER_H