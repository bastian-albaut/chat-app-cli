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
    send_message(*socketServer, message, "Message sent to the server\n");

  }
  pthread_exit(0);
}

/**** Choose and send pseudo to the server ****/
void send_pseudo() {

  int pseudoIsOk = 0;

  while(!pseudoIsOk) {
    // Send the pseudo to the server
    char pseudo[NB_CHARACTERS_PSEUDO];
    printf("Please enter your pseudo:\n");
    fgets(pseudo, NB_CHARACTERS_PSEUDO, stdin);
    char *findReturn = strchr(pseudo,'\n'); // Return null if not found
    if(findReturn != NULL) {
      *findReturn = '\0';
    }

    send_message(socketServerFromClient, pseudo, NULL);

    // Receive the response from the server 
    char response[NB_CHARACTERS];
    
    recv_message(socketServerFromClient, response);

    if(strcmp(response, "SUCCESS") == 0) {
      printf("Pseudo accepté\n");
      pseudoIsOk = 1;
    } else {
      printf("Pseudo déjà utilisé\n");
    }
  }
}

void interrupt_handler(int signal) {
  printf("\n");
  close_socket(socketServerFromClient);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF CLIENT ==========\n");
  exit(0);
}