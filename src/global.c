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