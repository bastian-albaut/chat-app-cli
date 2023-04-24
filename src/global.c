#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../include/global.h"
#include "../include/list.h"

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