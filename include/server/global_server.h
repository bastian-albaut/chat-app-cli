#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "../list/list.h"
#include "../global.h"

// Structure to store the arguments of the thread
typedef struct ThreadArgs {
    pthread_t* ptrTempIdThread;
    int socketClient;
} ThreadArgs;

/**
 * Function thread to manage each client
 *
 * @param args An instance of ThreadArgs structure with socketClient and listClient
 *
 * @return void
 */
extern void* thread_client(void* args);

/**
 * Name the socket of the server
 *
 * @param port The port of the socket
 *
 * @return void
 */
extern void name_socket_server(char* port);

/**
 * Listen the socket server
 *
 * @return description
 */
extern void listen_socket_server();

/**
 * handle acceptation of client
 *
 * @return The socket client if acceptation work correctly | -1 if not
 */
extern int accept_client();

/**
 * Handle the message of the client
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 * @param pseudo The pseudo of the client who send the message
 *
 * @return void
 */
extern void handle_message(char* message, int socketClient, char* pseudo, pthread_t threadId);

/**
 * Detect if the message corresponding to a special command (start with "/")
 *
 * @param param message The string to check
 *
 * @return 1 if the message is a special command | 0 if the message is not a special command
 */
extern char is_special_command(char* message);

/**
 * Function to handle the SIGINT signal (CTRL + C)
 *
 * @param signal The signal to handle
 *
 * @return void
 */
extern void interrupt_handler(int signal);

/**
 * Close all clients thread/sockets in the list
 *
 * @return void
 */
extern void close_all_clients();

/**
 * Remove the client from the list, close the socket corresponding and leave a place in the semaphore
 *
 * @param socketClient The socket of the client to remove
 *
 * @return void
 */
extern void remove_client(int socketClient, pthread_t threadId);

/**
 * Receive a message from the client with the socket specified in parameter
 *
 * @param socket The socket of the client to recv the message
 * @param message The message to store the received message
 *
 * @return The number of bytes received
 */
extern int recv_message(int socket, char* message);

/**
 * Send an informative, a success, a redirection or an error response of the client with the socket specified in parameter
 *
 * @param socketClient The socket of the client to send the response
 * @param code The code of the response
 * @param message The message of the response
 * @param pseudo The pseudo of the client who send the message (If the response is a redirection)
 *
 * @return 1 if the response is send | -1 if the response is not send
 */
extern int send_response(int socketClient, int code, char* message, char* pseudo);

/**
 * Serialize all the fields of the response into a buffer specified in parameter
 *
 * @param response The response to serialize
 * @param buffer The buffer to store the serialized response
 * @param sizeBuffer The length of the buffer
 *
 * @return void
 */
extern void serialize_response(Response* response, char* buffer, size_t sizeBuffer);

#endif // SERVER_FUNCTIONS_H
