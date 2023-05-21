#include "../include/global.h"
#include "../include/list/list.h"
#include "../include/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int socketServer;
Node* listClient;


void init_socket(int* newSocket, int isCreation, int isPrint) {
  if(isPrint) {
    printf("Socket initialization...\n");
  }

  *newSocket = socket(AF_INET, SOCK_STREAM, 0);

  if(*newSocket == -1) {
    perror("Error: Socket creation");
    exit(1);
  }

  // Allow to use address again
  if(isCreation) {
    int optval = 1;
    setsockopt(*newSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  }

  if(isPrint) {
    printf(" => Socket created\n");
  }
}


void connection_request(int* socket, char* ipAdress, int port, int isPrint) {
  struct sockaddr_in adress;

  adress.sin_family = AF_INET;
  inet_pton(AF_INET, ipAdress, &(adress.sin_addr));
  adress.sin_port = htons(port);
  
  socklen_t sizeAdress = sizeof(adress);
  
  if(connect(*socket, (struct sockaddr *) &adress, sizeAdress) == -1){
    perror("Error: Server connection request");
    exit(1);
  }

  if(isPrint) {
    printf(" => Connection socket accepted\n");
  }
}


void name_socket(int* socket, int port, int isPrint) {
  struct sockaddr_in adress;
  socklen_t sizeAdress= sizeof(adress);

  adress.sin_family = AF_INET; 
  adress.sin_addr.s_addr = INADDR_ANY;
  adress.sin_port = htons(port);

  if(bind(*socket, (struct sockaddr*)&adress, sizeAdress) == -1) {
    perror("Error: Socket naming");
    exit(1);
  }

  if(isPrint) {
    printf(" => Socket named\n");
  }
}


void listen_socket(int* socket, int capacityWait, int isPrint) {
  if(listen(*socket, capacityWait) == -1) {
    perror("Error: Socket listening");
    exit(1);
  }

  if(isPrint) {
    printf(" => Socket listening\n\n");
  }
}


void close_socket(int socket) {
  if(shutdown(socket, 2)) {
    perror("Error: Close socket");
    exit(1);
  }
}




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