#ifndef UPLOAD_FILE_CLIENT_H
#define UPLOAD_FILE_CLIENT_H

// Structure of the arguments of the thread
typedef struct ThreadArgsRecvFile {
    int socketFile;
    int sizeFile;
    char* fileName;
} ThreadArgsRecvFile;

/**
 * Detect if the message corresponding to a send file message (start with "/sendfile")
 * 
 * @param message The string to check
 * 
 * @return 1 if the message is a send file message | 0 if the message is not a send file message
 */
extern int is_send_file_message(char* message);

/**
 * Handle send file message. Send a message to the client to confirm the send of the file.
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who send the request of send file
 *
 * @return description
 */
extern void handle_send_file_message(char* message, int socketClient);

/**
 * Check if the file name and the size of the file are correct
 *
 * @param fileName The name of the file
 * @param size The size of the file
 * @param message The message to send to the client if the file name or the size of the file are not correct
 *
 * @return 1 if the file name and the size of the file are correct | 0 if not
 */
extern int is_file_name_and_size_ok(char* fileName, int size, char* message);

/**
 * Detect if the message corresponding to the format "/sendfile <file_name> <file_size>"
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_send_file_message(char* message);

/**
 * Get the name and the size of the file
 *
 * @param message The string to split to get the name and the size of the file
 * @param file_name The name of the file
 * @param file_size The size of the file
 *
 * @return void
 */
extern void get_file_name_and_size(char* message, char** file_name, int* file_size);

/**
 * Function thread to receive the content of the file
 *
 * @param agrs The arguments of the thread
 *
 * @return description
 */
extern void* thread_receive_file(void* args);

#endif // UPLOAD_FILE_CLIENT_H