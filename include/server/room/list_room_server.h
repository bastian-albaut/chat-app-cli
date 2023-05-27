#ifndef LIST_ROOM_SERVER_H
#define LIST_ROOM_SERVER_H

/**
 * Detect if the message corresponding to a list rooms message (Start with /listrooms)
 *
 * @param message The string to check
 *
 * @return 1 if the message is a list rooms message | 0 if the message is not a list rooms message
 */
extern int is_list_rooms_message(char* message);

/**
 * Handle a list rooms message
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who sent the message
 * 
 * @return void
 */
extern void handle_list_rooms_message(char* message, int socketClient);

/**
 * Detect if the message is in the good format (/listrooms)
 *
 * @param message The message to check
 *
 * @return 1 if the message is in the good format | 0 if the message is not in the good format
 */
extern int is_good_format_list_rooms(char* message);

#endif // LIST_ROOM_SERVER_H