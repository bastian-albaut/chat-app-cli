#ifndef LIST_FILES_SERVER_H
#define LIST_FILES_SERVER_H

/**
 * Detect if the message corresponding to a list files server message (start with "/listfiles server")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a list files server message | 0 if the message is not a list files server message
 */
extern int is_list_files_server(char* message);

/**
 * Handle list files server message. Send a message to the client to give him the list of files in the server directory.
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who send the request of list files server
 *
 * @return void
 */
extern void handle_list_files_server(char* message, int socketClient);

/**
 * Detect if the message corresponding to the format "/listfiles server"
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_list_files_server(char* message);

/**
 * Get the list of files in the server directory
 *
 * @return The list of files
 */
extern char* get_list_files();

#endif // LIST_FILES_SERVER_H