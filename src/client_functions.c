#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>
#include "../include/global.h"
#include "../include/client_functions.h"
#include "../include/constants.h"

/* Global variables */

// Socket of the server get from the client
int socketServerFromClient;

// Semaphore for the capacity of the server
int idSemaphore;


/**
 * Function thread for sending messages to the server
 *
 * @param socket The socket of the server
 *
 * @return void
 */
void* thread_send(void *socket) {
  int *socketServer = (int*) socket;

  // Loop to send message(s) to the server
  while(1) {

    // Preparing to send the message to the server
    char message[NB_CHARACTERS];
    get_input(message, NB_CHARACTERS, NULL);

    // Sending the message to the server
    send_message(*socketServer, message, NULL);

  }
}


/**
 *
 * Initialize the socket of the client
 *
 * @return void
 */
void init_socket_client() {

  socketServerFromClient = socket(PF_INET, SOCK_STREAM, 0);

  if(socketServerFromClient == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }
}


/**
 * Send a connection request to the server
 *
 * @param ipAdress The ip adress of the server
 * @param port The port of the server
 *
 * @return void
 */
void connection_request(char* ipAdress, char* port) {
  struct sockaddr_in adress;

  adress.sin_family = AF_INET;
  inet_pton(AF_INET, ipAdress, &(adress.sin_addr));
  adress.sin_port = htons(atoi(port));
  
  socklen_t sizeAdress = sizeof(adress);
  
  if(connect(socketServerFromClient, (struct sockaddr *) &adress, sizeAdress) == -1){
    perror("Error: Server connection request");
    exit(1);
  }

  printf(BOLD "\n==================================\n");
  printf("You are connected to the server :)\n");
  printf("==================================\n\n" RESET);
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
    char pseudo[NB_CHARACTERS];

    get_input(pseudo, NB_CHARACTERS, "Please enter your pseudo:");

    send_message(socketServerFromClient, pseudo, NULL);

    // Receive the response from the server 
    Response* response = malloc(sizeof(Response));
    int nbByteRead = recv_response(socketServerFromClient, response);

    if(nbByteRead == 0 || nbByteRead == -1) {
      printf("The connection was cut on the server side\n");
      exit(1);
    }

    if(response->code == PSEUDO_ACCEPTED) {
      pseudoIsOk = 1;
    }
    print_response(response);
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
  leave_place_semaphore(idSemaphore);
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


/**
 * Send a message to the server with the socket specified in parameter
 *
 * @param socketServer The socket of the server to send the message
 * @param message The message to send
 * @param prompt The prompt to display after sending the message
 *
 * @return void
 */
void send_message(int socketServer, char* message, char* prompt) {
  if(send(socketServer, message, strlen(message)+1, 0) == -1) {
    perror("Error: Send message");
    exit(1);
  }
  if(prompt != NULL) {
    printf("%s\n", prompt);
  }
}


/**
 * Recv an informative, a success, a redirection or an error response of the server with the socket specified in parameter
 *
 * @param socketServer The socket of the server to recv the message
 * @param response The response to store the message
 *
 * @return The number of bytes received
 */
int recv_response(int socketServer, Response* response) {

  char buffer[RESPONSE_BUFFER_SIZE];
  int nbBytes = recv(socketServer, buffer, sizeof(buffer), 0);
  if(nbBytes == -1) {
    perror("Error: Receiving the response");
    exit(1);
  }

  if(nbBytes == 0) {
    return 0;
  }

  // Deserialize the response
  deserialize_response(buffer, sizeof(buffer), response);

  return nbBytes;
}


/**
 * Deserialize a response from the buffer specified in parameter and store it in an instance of Response
 *
 * @param buffer The buffer to deserialize
 * @param sizeBuffer The length of the buffer
 * @param response The response to store the deserialized buffer
 *
 * @return void
 */
void deserialize_response(char* buffer, size_t sizeBuffer, Response* response) {
  
  // Unpack the code field from the buffer
  int code;
  memcpy(&code, buffer, sizeof(code));
  response->code = ntohl(code);
  buffer += sizeof(code);
  sizeBuffer -= sizeof(code);

  // Allocate memory for the message field and copy the serialized message into it
  size_t lengthMessage = strlen(buffer) + 1; 
  response->message = malloc(lengthMessage);
  if (response->message == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }
  memcpy(response->message, buffer, lengthMessage);
  buffer += lengthMessage;
  sizeBuffer -= lengthMessage;

  // Allocate memory for the from field and copy the serialized from field into it
  size_t lengthFrom = strlen(buffer) + 1;
  response->from = malloc(lengthFrom);
  if (response->from == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }
  memcpy(response->from, buffer, lengthFrom);
}


/**
 * Print the response specified in parameter with a color depending on the code of the response
 *
 * @param response The response to print
 *
 * @return void
 */
void print_response(Response* response) {
  if(response->code >= 100 && response->code < 200) {
    printf(BLUE "%d - %s\n" RESET, response->code, response->message);
  } else if(response->code >= 200 && response->code < 300) {
    if(response->code == 203) {
      printf(GREEN "%s\n" RESET, response->message);
    } else {
      printf(GREEN "%d - %s\n" RESET, response->code, response->message);
    }
  } else if(response->code >= 300 && response->code < 400) {
    if(response->code == MESSAGE_GLOBAL_REDIRECT) {
      printf(YELLOW "%s: %s\n" RESET, response->from, response->message);
    } else if(response->code == MESSAGE_PRIVATE_REDIRECT) {
      printf(YELLOW "%s (Message Privé): %s\n" RESET, response->from, response->message);
    } else {
      printf(YELLOW "%d - %s\n" RESET, response->code, response->message);
    }
  } else {
    printf(RED "%d - %s\n" RESET, response->code, response->message);
  }
}


/**
 * Initialize the semaphore for handle capacity of clients
 *
 * @return void
 */
void init_semaphore_client() {
  int keySemaphore = ftok("command.txt", 'r');
  if (keySemaphore == -1) {
    perror("Error ftok");
    exit(1);
  }

  int nbSemaphores = 1;  
  idSemaphore = semget(keySemaphore, nbSemaphores, 0666);
  if(idSemaphore == -1){
    perror("Error semget");
    exit(1);
  }
}



/**
 * Decrement the semaphore in global variables to take place in it
 *
 * @return void
 */
void take_place_semaphore() {
  
  struct sembuf operation = {0, -1, IPC_NOWAIT}; // Decrement the semaphore (non-blocking)
  
  if (semop(idSemaphore, &operation, 1) == -1) {
    if (errno == EAGAIN) {
      // No place available in the semaphore
      printf("\n\n\nNo place available in the semaphore. Please try again later.\n");
      exit(0);
    }
    perror("Error: Taking place in the semaphore");
    exit(1);
  }
}