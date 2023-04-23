#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/global.h"
#include "../include/client_functions.h"
#include "../include/constants.h"

/**** Global variables ****/
int socketServerFromClient;

/**** Thread for sending messages to the server ****/
void* thread_send(void *socket) {
  int *socketServer = (int*) socket;

  while(1) {

    /**** Preparing to send the message to the server ****/
    char message[NB_CHARACTERS];
    printf("Please enter the message you want to send:\n");
    fgets(message, NB_CHARACTERS, stdin);
    char *findReturn = strchr(message,'\n'); // Return null if not found
    if(findReturn != NULL) {
      *findReturn = '\0';
    }

    /**** Sending the message to the server ****/
    if(send(*socketServer, message, strlen(message)+1, 0) == -1){
      perror("Error: Send message");
      exit(1);
    }
    printf("Message sent to the server\n");

  }
  pthread_exit(0);
}

void interrupt_handler(int signal) {
  printf("\n");
  close_socket(socketServerFromClient);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF CLIENT ==========\n");
  exit(0);
}