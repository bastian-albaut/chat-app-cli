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

/* Global variables */

// Socket of the server get from the client
int socketServerFromClient;


/**
 * Function thread for sending messages to the server
 *
 * @param socket The socket of the server
 *
 * @return void
 */
void* thread_send(void *socket) {
  int *socketServer = (int*) socket;

  while(1) {

    // Preparing to send the message to the server
    char message[NB_CHARACTERS];
    get_input(message, NB_CHARACTERS, NULL);

    // Sending the message to the server
    send_message(*socketServer, message, "Message sent to the server");

  }
  pthread_exit(0);
}


/**
 * Choose and send pseudo to the server
 *
 * @return void
 */
void send_pseudo() {

  int pseudoIsOk = 0;

  while(!pseudoIsOk) {
    // Send the pseudo to the server
    char pseudo[NB_CHARACTERS_PSEUDO];

    get_input(pseudo, NB_CHARACTERS_PSEUDO, "Please enter your pseudo:");

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


/**
 * Function to handle the SIGINT signal (CTRL + C)
 *
 * @param signal The signal to handle
 *
 * @return void
 */
void interrupt_handler(int signal) {
  printf("\n");
  close_socket(socketServerFromClient);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF CLIENT ==========\n");
  exit(0);
}


/**
 * Get the input of the user
 *
 * @param message The message to store the input
 * @param size The size of the message
 * @param prompt The prompt to display before getting the input
 *
 * @return void
 */
void get_input(char* message, int size, char* prompt) {
  if(prompt != NULL) {
    printf("%s\n", prompt);
  }

  fgets(message, size, stdin);
  char *findReturn = strchr(message,'\n'); // Return null if not found
  if(findReturn != NULL) {
    *findReturn = '\0';
  }
}