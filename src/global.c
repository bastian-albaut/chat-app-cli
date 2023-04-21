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