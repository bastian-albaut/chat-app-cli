#include "../../include/client/global_client.h"
#include "../../include/client/semaphore/semaphore_client.h"
#include "../../include/client/file/send_file_client.h"
#include "../../include/client/file/recv_file_client.h"
#include "../../include/client/file/list_files_client.h"
#include "../../include/constants.h"
#include "../../include/global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int socketServerFromClient;
int idSemaphore;

void interrupt_handler(int signal) {
  printf("\n");
  close_socket(socketServerFromClient);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF CLIENT ==========\n");
  exit(0);
}


void* thread_send(void *socket) {
  int *socketServer = (int*) socket;

  // Loop to send message(s) to the server
  while(1) {

    // Preparing to send the message to the server
    char message[NB_CHARACTERS];
    get_input(message, NB_CHARACTERS, NULL);

    handle_message(message, *socketServer);
  }
}


void init_socket_client() {

  socketServerFromClient = socket(PF_INET, SOCK_STREAM, 0);

  if(socketServerFromClient == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }
}


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

  printf(" __        __   _                            \n");
  printf(" \\ \\      / /__| | ___ ___  _ __ ___   ___  \n");
  printf("  \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ \n");
  printf("   \\ V  V /  __/ | (_| (_) | | | | | |  __/ \n");
  printf("    \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___| \n");
  printf("                                                    \n");
  printf(BOLD "            Welcome to the server!               \n\n\n" RESET);
}


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


void send_message(int socketServer, char* message, char* prompt) {
  if(send(socketServer, message, strlen(message)+1, 0) == -1) {
    perror("Error: Send message");
    exit(1);
  }
  if(prompt != NULL) {
    printf("%s\n", prompt);
  }
}


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


void print_response(Response* response) {

  // Informative response
  if(response->code >= 100 && response->code < 200) {
    printf(BLUE "%d - %s" RESET, response->code, response->message);
  } 
  
  // Success response
  if(response->code >= 200 && response->code < 300) {

    if(response->code == PSEUDO_ACCEPTED) {
      printf(GREEN "%d - %s" RESET, response->code, response->message);
      printf(GREY "\n\nType /help to see the list of commands" RESET);
      printf("\n\n");
      return;
    }

    if(response->code == HELP_SUCCESS) {
      printf(GREEN "%s" RESET, response->message);
      printf("\n");
      return;
    } 
    
    if(response->code == LIST_FILE_SUCCESS) {
      printf(GREEN "\n------ List of server file(s) ------\n");
      printf("%s", response->message);
      printf("------ End list ------" RESET);
      printf("\n\n");
      return;
    }

    printf(GREEN "%d - %s" RESET, response->code, response->message);
  } 

  // Redirection response
  if(response->code >= 300 && response->code < 400) {

    if(response->code == MESSAGE_GLOBAL_REDIRECT) {
      printf(YELLOW "%s: %s" RESET, response->from, response->message);
      printf("\n");
      return;
    } 

    if(response->code == MESSAGE_PRIVATE_REDIRECT) {
      printf(YELLOW "%s (Message Privé): %s" RESET, response->from, response->message);
      printf("\n");
      return;
    }

    printf(YELLOW "%d - %s" RESET, response->code, response->message);
  } 
  
  // Error response
  if(response->code >= 400 && response->code < 500) {
    printf(RED "%d - %s" RESET , response->code, response->message);
  }
  printf("\n");
}



void handle_message(char* message, int socketServer) {
  if(is_list_file_message(message)) {
    handle_list_file_message(message);
    return;
  }

  if(is_send_file_message(message)) {
    handle_send_file_message(message, socketServer);
    return;
  }

  if(is_recv_file_message(message)) {
    handle_recv_file_message(message, socketServer);
    return;
  }

  // Sending the message to the server
  send_message(socketServer, message, NULL);
}


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