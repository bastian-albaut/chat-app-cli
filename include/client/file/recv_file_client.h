#ifndef RECV_FILE_CLIENT_H
#define RECV_FILE_CLIENT_H

// Structure of the arguments of the thread
typedef struct ThreadArgsRecvFile {
  int socketServer;
  char* message;
} ThreadArgsRecvFile;

/**
 * Detect if the message corresponding to recv file command (start with "/recvfile")
 *
 * @param message The message to check
 *
 * @return 1 if the message is a recv file command | 0 if the message is not a recv file command
 */
extern int is_recv_file_message(char* message);

/**
 * Handle the recv file command
 *
 * @param message The message to handle
 * @param socketServer The socket of the server
 *
 * @return void
 */
extern void handle_recv_file_message(char* message, int socketServer);

/**
 * Detect if the message corresponding to the format /recvfile <file_name>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
extern int is_good_format_recv_file_message(char* message);

/**
 * Function thread for handle file transfer
 *
 * @param args The arguments of the thread
 *
 * @return void
 */
extern void* thread_recv_file(void* args);

#endif // RECV_FILE_CLIENT_H