#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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
  printf("Socket %d closed\n", socket);
}


/**
 * Send a message to the socket specified in parameter
 *
 * @param socket The socket to send the message
 * @param message The message to send
 * @param prompt The prompt to display after sending the message
 *
 * @return void
 */
void send_message(int socket, char* message, char* prompt) {
  if(send(socket, message, strlen(message)+1, 0) == -1) {
    perror("Error: Send message");
    exit(1);
  }
  if(prompt != NULL) {
    printf("%s\n", prompt);
  }
}


/**
 * Receive a message from the socket specified in parameter
 *
 * @param socket The socket to recv the message
 * @param message The message to store the received message
 *
 * @return The number of bytes received
 */
int recv_message(int socket, char* message) {
  int nbByteRead = 0;

  nbByteRead = recv(socket, message, NB_CHARACTERS, 0);

  if(nbByteRead == -1) {
    perror("Error: Receiving the message");
    exit(1);
  }

  return nbByteRead;
}