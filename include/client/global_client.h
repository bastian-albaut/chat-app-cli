#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

#include "../../include/global.h"


// Socket of the server get from the client
extern int socketServerFromClient;

// Semaphore for the capacity of the server
extern int idSemaphore;

/**
 * Function to handle the SIGINT signal (CTRL + C)
 *
 * @param signal The signal to handle
 *
 * @return void
 */
extern void interrupt_handler(int signal);

/**
 * Function thread for sending messages to the server
 *
 * @param socket The socket of the server
 *
 * @return void
 */
extern void* thread_send(void *socket);

/**
 * Get the input of the user
 *
 * @param message The message to store the input
 * @param size The size of the message
 * @param prompt The prompt to display before getting the input
 *
 * @return void
 */
extern void get_input(char* message, int size, char* prompt);

/**
 * Send a message to the server with the socket specified in parameter
 *
 * @param socketServer The socket of the server to send the message
 * @param message The message to send
 * @param prompt The prompt to display after sending the message
 *
 * @return void
 */
extern void send_message(int socket, char* message, char* prompt);

/**
 * Recv an informative, a success, a redirection or an error response of the server with the socket specified in parameter
 *
 * @param socketServer The socket of the server to recv the message
 * @param response The response to store the message
 *
 * @return The number of bytes received
 */
extern int recv_response(int socketServer, Response* response);

/**
 * Deserialize a response from the buffer specified in parameter and store it in an instance of Response
 *
 * @param buffer The buffer to deserialize
 * @param sizeBuffer The length of the buffer
 * @param response The response to store the deserialized buffer
 *
 * @return void
 */
extern void deserialize_response(char* buffer, size_t sizeBuffer, Response* response);

/**
 * Print the response specified in parameter with a color depending on the code of the response
 *
 * @param response The response to print
 *
 * @return void
 */
extern void print_response(Response* response);

/**
 * Handle the message specified in parameter
 *
 * @param message The message to handle
 * @param socketServer The socket of the server
 *
 * @return void
 */
extern void handle_message(char* message, int socketServer);

/**
 * Choose and send pseudo to the server
 *
 * @return void
 */
extern void send_pseudo();

#endif // CLIENT_FUNCTIONS_H