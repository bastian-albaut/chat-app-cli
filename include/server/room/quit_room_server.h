#ifndef QUIT_ROOM_SERVER_H
#define QUIT_ROOM_SERVER_H

/**
 * Detect if the message corresponding to a quit room message (Start with /quitroom)
 *
 * @param message The string to check
 *
 * @return 1 if the message is a quit room message | 0 if the message is not a quit room message
 */
extern int is_quit_room_message(char* message);

/**
 * Handle a quit room message
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who sent the message
 * @param pseudoClient The pseudo of the client who sent the message
 * 
 * @return void
 */
extern void handle_quit_room_message(char* message, int socketClient, char* pseudoClient);

/**
 * Detect if the message is in the good format (/quitroom <room_name>)
 *
 * @param message The message to check
 *
 * @return 1 if the message is in the good format | 0 if the message is not in the good format
 */
extern int is_good_format_quit_room(char* message);

/**
 * Get the room name from a quit room message
 *
 * @param message The message to check
 *
 * @return The room name
 */
extern char* get_room_name(char* message);

#endif // QUIT_ROOM_SERVER_H