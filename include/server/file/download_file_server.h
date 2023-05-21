#ifndef DOWNLOAD_FILE_SERVER_H
#define DOWNLOAD_FILE_SERVER_H

// Structure of the arguments of the thread
typedef struct ThreadArgsSendFile {
    int socket;
    char* fileName;
} ThreadArgsSendFile;

/**
 * Detect if the message corresponding to a recv files client message (start with "/recvfile")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a recv files client message | 0 if the message is not a recv files client message
 */
extern int is_recv_file_message(char* message);

/**
 * Handle recv file message. Send a message to the client to confirm the send of the file.
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who send the request of recv file
 *
 * @return void
 */
extern void handle_recv_file_message(char* message, int socketClient);

/**
 * Detect if the message corresponding to the format "/recvfile <file_name>"
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_recv_file_message(char* message);

/**
 * Return the file name from /recvfile <file_name>
 *
 * @param message The message to split
 *
 * @return The name of the file
 */
extern char* get_file_name_recv_file_message(char* message);

/**
 * Function thread to send the name, the size and the content of the file
 *
 * @param args The arguments of the thread
 *
 * @return void
 */
extern void* thread_send_file(void* args);

/**
 * Get the size of the file that correspond to the file name specified in parameter
 *
 * @param fileName The path of the file
 *
 * @return The size of the file
 */
extern int get_size_recv_file(char* fileName);

#endif // DOWNLOAD_FILE_SERVER_H