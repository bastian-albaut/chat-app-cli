#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include "../include/global.h"
#include "../include/list.h"
#include "../include/constants.h"

/* Global variables */

// Socket of the server
int socketServer;

// List of clients connected to the server
Node* listClient;


/**
 * Close the socket specified in parameter
 *
 * @param socket The socket to close
 *
 * @return void
 */
void close_socket(int socket) {
  if(shutdown(socket, 2)) {
    perror("Error: Close socket");
    exit(1);
  }
}


/**
 * Create the file with the name and the path specified in parameter
 *
 * @param fileName The name of the file to create
 * @param path The path where to create the file
 *
 * @return The file created
 */
FILE* create_file(char* fileName, char* path) {

  // Concatenate the directory and the file name
  char* filePath = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(filePath, path);
  strcat(filePath, fileName);

  FILE* file = fopen(filePath, "w");

  if(file == NULL) {
    perror("Error: opening file");
    exit(1);
  }

  printf("File \"%s\" created\n", fileName);

  return file;
}