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
    setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  }

  if(isPrint) {
    printf("Socket created\n");
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