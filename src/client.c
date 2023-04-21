#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/client_functions.h"
#include "../include/constants.h"
#include "../include/global.h"

int main(int argc, char *argv[]) {

  /**** Arguments verification ****/
  if(argc != 3){
    perror("Error: Expected arguments are <ip> <port>");
    exit(1);
  }

  /**** Creation of socket ****/
  int socketServer = socket(PF_INET, SOCK_STREAM, 0);
  if(socketServer == -1) {
    perror("Error: Creation of socket");
  }

  /**** Server connection request ****/
  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr));
  aS.sin_port = htons(atoi(argv[2]));
  socklen_t lgA = sizeof(struct sockaddr_in);
  
  if(connect(socketServer, (struct sockaddr *) &aS, lgA) == -1){
    perror("Error: Server connection request");
    exit(1);
  }
  printf("You are connected to the server :)\n");

  /**** Creation of a thread for sending message(s) to the server ****/
  pthread_t thread;
  pthread_create(&thread, NULL, thread_send, &socketServer);

  int nbByteRead = 1;
  while(nbByteRead != 0) {  

    /**** Receive message from server ****/
    char *message = malloc(NB_CHARACTERS * sizeof(char));
    nbByteRead = recv(socketServer, message, NB_CHARACTERS, 0);
    if(nbByteRead == -1) {
      perror("Error: Receiving the message");
      exit(1);
    } else if(nbByteRead == 0) {
      printf("The connection was cut on the server side\n");
    } else {
      printf("Message receive: %s\n", message);
    }
  }

  /**** Closing the socket ****/
  close_socket(socketServer);

  printf("End of program\n");
}