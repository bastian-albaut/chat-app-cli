#ifndef GLOBAL_H
#define GLOBAL_H

#include "./list/list.h"
#include "constants.h"
#include <stdio.h>

// Structure of a response of the server
typedef struct Response {
  int code;
  char* message;
  char* from;
} Response;

// Socket of the server
extern int socketServer;

// List of clients connected to the server
extern Node* listClient;

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