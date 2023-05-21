#ifndef SEND_FILE_CLIENT_H
#define SEND_FILE_CLIENT_H

/**
 * Detect if the message corresponding to send file command (start with "/sendfile")
 *
 * @param message The message to check
 *
 * @return 1 if the message is a send file command | 0 if the message is not a send file command
 */
extern int is_send_file_message(char* message);

/**
 * Handle the send file command
 *
 * @param message The message to handle
 * @param socketServer The socket of the server
 *
 * @return void
 */
extern void handle_send_file_message(char* message, int socketServer);

/**
 * Detect if the message corresponding to the format /sendfile <file_name>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_send_file_message(char* message);

/**
 * Get the file name in the message
 *
 * @param message The message to check
 *
 * @return The file name in the message
 */
extern char* get_file_name(char* message);

/**
 * Detect if the file specified in parameter exist
 *
 * @param fileName The name of the file to check
 *
 * @return 1 if the file exist | 0 if the file doesn't exist
 */
extern int is_file_exist(char* fileName);

/**
 * Send name and size of the file specified in parameter to the server 
 * Create a new socket to handle the file transfer
 * Send the content of the file by blocks of 1024 bytes with the new socket
 *
 * @param socketServer The socket of the server
 * @param fileName The name of the file to send
 *
 * @return void
 */
extern void send_file(int socketServer, char* fileName);

/**
 * Function thread for handle file transfer
 *
 * @param socket The socket of the server
 *
 * @return void
 */
extern void* thread_file_transfer(void *arg);

/**
 * Name the socket of the file transfer
 *
 * @param socket The socket of the file transfer
 * @param port The port of the socket
 *
 * @return void
 */
extern void name_socket_file(int socketFile, int port);

/**
 * Listen the socket for file transfer
 *
 * @return void
 */
extern void listen_socket_file(int socketFile);

#endif // SEND_FILE_CLIENT_H