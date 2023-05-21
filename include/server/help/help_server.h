#ifndef HELP_SERVER_H
#define HELP_SERVER_H

/**
 * Detect if the message corresponding to a help message (start with "/help")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a help message | 0 if the message is not a help message
 */
extern int is_help_message(char* message);

/**
 * Handle help message. Send a message to the client to give him the list of commands.
 *
 * @param message The message to handle 
 * @param socketClient The socket of the client who send the request of help
 *
 * @return void
 */
extern void handle_help_message(char* message, int socketClient);

/**
 * Detect if the message corresponding to the format /help
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_help_message(char* message);

/**
 * Get the content of the file specified in parameter
 *
 * @param filename The name of the file to read
 * 
 * @return The content of the file
 */
extern char* get_content_of_file(char* filename);

#endif // HELP_SERVER_H