#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../include/global.h"
#include "../include/list.h"
#include "../include/constants.h"

/**** Global variables ****/
int socketServer;
Node* listClient;

/**** Close the socket specified in parameter ****/
void close_socket(int socket) {
  if(shutdown(socket, 2)) {
    perror("Error: Close socket");
    exit(1);
  }
  printf("Socket %d closed\n", socket);
}

/**** Send a message to the socket specified in parameter ****/
void send_message(int socket, char* message, char* prompt) {
  if(send(socket, message, strlen(message)+1, 0) == -1) {
    perror("Error: Send message");
    exit(1);
  }
  if(prompt != NULL) {
    printf("%s", prompt);
  }
}

/**** Receive a message from the socket specified in parameter ****/
// Return the number of bytes received and exit if an error occurs
int recv_message(int socket, char* message) {
  int nbByteRead = 0;

  nbByteRead = recv(socket, message, NB_CHARACTERS, 0);

  if(nbByteRead == -1) {
    perror("Error: Receiving the message");
    exit(1);
  }

  return nbByteRead;
}

/**** Get the input of the user ****/
void get_input(char* message, int size, char* prompt) {
  if(prompt != NULL) {
    printf("%s", prompt);
  }

  fgets(message, size, stdin);
  char *findReturn = strchr(message,'\n'); // Return null if not found
  if(findReturn != NULL) {
    *findReturn = '\0';
  }
}