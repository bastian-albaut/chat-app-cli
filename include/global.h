#ifndef GLOBAL_H
#define GLOBAL_H

#include "./list/list.h"
#include "constants.h"
#include <stdio.h>

// Structure of a response of the server
typedef struct Response {
  ErrorCode code;
  char* message;
  char* from;
} Response;

// Socket of the server
extern int socketServer;

// List of clients connected to the server
extern Node* listClient;

/**
 * Initialize the socket specified in parameter in TCP mode
 *
 * @param newSocket The socket to initialize
 * @param isCreation If the initialisation corresponding to the creation of the socket
 * @param isPrint If the function have to print the message
 *
 * @return void
 */
extern void init_socket(int* newSocket, int isCreation, int isPrint);

/**
 * Send a connection request to a socket
 *
 * @param socket The socket to send the connection request
 * @param ipAdress The ip adress of the instance to connect
 * @param port The port of the instance to connect
 * @param isPrint If the function have to print the message
 *
 * @return void
 */
extern void connection_request(int* socket, char* ipAdress, int port, int isPrint);

/**
 * Close the socket specified in parameter
 *
 * @param socket The socket to close
 *
 * @return void
 */
extern void close_socket(int socket);

/**
 * Create the file with the name and the path specified in parameter
 *
 * @param fileName The name of the file to create
 * @param path The path where to create the file
 *
 * @return The file created
 */
extern FILE* create_file(char* fileName, char* path);

#endif // GLOBAL_H