#include "../../include/server/global_server.h"
#include "../../include/server/message/global_message_server.h"
#include "../../include/server/message/private_message_server.h"
#include "../../include/server/pseudo/pseudo_server.h"
#include "../../include/server/file/download_file_server.h"
#include "../../include/server/file/upload_file_server.h"
#include "../../include/server/file/list_files_server.h"
#include "../../include/server/logout/logout_server.h"
#include "../../include/server/help/help_server.h"
#include "../../include/server/semaphore/semaphore_server.h"
#include "../../include/list/list.h"
#include "../../include/constants.h"
#include "../../include/global.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


void* thread_client(void* args) {
  ThreadArgs* data = (ThreadArgs*) args;
  int socketClient = data->socketClient;
  pthread_t* ptrTempIdThread = data->ptrTempIdThread;

  // Get pseudo of the client 
  char* pseudo = malloc(NB_CHARACTERS_PSEUDO_MAX * sizeof(char));
  if(handle_pseudo_client(socketClient, pseudo) == -1) {
    remove_client(socketClient, *ptrTempIdThread);
    pthread_exit(0);
  }

  // Add the client to the list
  Node* currentClient = insert_first(&listClient, socketClient, pseudo, *ptrTempIdThread);

  // Inform the server that a new client is connected
  printf("Client %s connected !\n", pseudo);
  display_list(&listClient);

  // Loop to receive message and handle messages from client
  while(1) {

    // Receive message from client 
    char message[NB_CHARACTERS];
    int nbByteRead = recv_message(socketClient, message);

    // If the client is disconnected
    if(nbByteRead == 0 || nbByteRead == -1) {
      remove_client(socketClient, *ptrTempIdThread);
      break;
    } 
    
    printf("Message receive: %s\n", message);

    handle_message(message, socketClient, pseudo, pthread_self());
  }
}


int accept_client() {
  struct sockaddr_in adressClient;
  socklen_t sizeAdressClient = sizeof(adressClient);

  int socketClient = accept(socketServer, (struct sockaddr*)&adressClient, &sizeAdressClient);

  return socketClient;
}


void handle_message(char* message, int socketClient, char* pseudo, pthread_t threadId) {
  if(is_special_command(message)) {
    if(is_global_message(message)) {
      handle_global_message(message, socketClient, pseudo);
      return;
    }

    if(is_private_message(message)) {
      handle_private_message(message, socketClient, pseudo);
      return;
    }

    if(is_logout_message(message)) {
      handle_logout_message(message, socketClient, threadId);
      return;
    }

    if(is_help_message(message)) {
      handle_help_message(message, socketClient);
      return;
    }

    if(is_send_file_message(message)) {
      handle_send_file_message(message, socketClient);
      return;
    }

    if(is_list_files_server(message)) {
      handle_list_files_server(message, socketClient);
      return;
    }

    if(is_recv_file_message(message)) {
      handle_recv_file_message(message, socketClient);
      return;
    }
  }
  char* responseMessage = "Command not found";
  send_response(socketClient, COMMAND_NOT_FOUND, responseMessage, NULL);
}


char is_special_command(char* message) {
  if (strncmp(message, "/", 1) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void interrupt_handler(int signal) {
  printf("\n");
  close_all_clients();
  printf("  =>  All client threads/sockets are closed\n");
  close_socket(socketServer);
  printf("  =>  Server socket is closed\n");
  deletion_semaphore();
  printf("  =>  Semaphore is deleted\n");
  printf("\n========== END OF SERVER ==========\n");
  exit(0);
}


void close_all_clients() {
  Node* currentClient = listClient->next;
  while(currentClient != listClient) {

    // Cancel the thread
    if(pthread_cancel(currentClient->thread) != 0) {
      perror("Error: Canceling the thread");
      exit(1);
    }

    // Close the socket
    close_socket(currentClient->number);

    currentClient = currentClient->next;
  }
}


void remove_client(int socketClient, pthread_t threadId) {
  
  // Leave a place in the semaphore
  leave_place_semaphore();

  // Remove the client from the list
  remove_element(&listClient, search_element(&listClient, socketClient));

  // Close the socket
  close_socket(socketClient);

  printf("Client %d disconnected\n", socketClient);
  display_list(&listClient);

  // Cancel the thread
  if(pthread_cancel(threadId) != 0) {
    perror("Error: Canceling the thread");
    exit(1);
  }
}


int recv_message(int socketClient, char* message) {
  int nbByteRead = recv(socketClient, message, NB_CHARACTERS, 0);
  if(nbByteRead == -1) {
    perror("Error: Receiving the message");
    exit(1);
  }
  return nbByteRead;
}


int send_response(int socketClient, int code, char* message, char* pseudo) {
  Response* response = malloc(sizeof(Response));

  // Initialise the code field
  response->code = code;

  // Initialise the message field
  response->message = malloc(strlen(message) + 1);
  strcpy(response->message, message);

  // Initialise the from field
  if(pseudo != NULL) {
    response->from = malloc(strlen(pseudo) + 1);
    strcpy(response->from, pseudo);
  } else {
    response->from = malloc(strlen(RESPONSE_FROM_SERVER) + 1);
    strcpy(response->from, RESPONSE_FROM_SERVER);
  }

  // Serialize the response
  char buffer[RESPONSE_BUFFER_SIZE];
  serialize_response(response, buffer, sizeof(buffer));

  // Send the response
  if(send(socketClient, buffer, sizeof(buffer), 0) == -1) {
    perror("Error: Sending the response");
    return -1;
  }

  return 1;
}


void serialize_response(Response* response, char* buffer, size_t sizeBuffer) {
  
  // Pack the code field into the buffer
  int code = htonl(response->code);
  memcpy(buffer, &code, sizeof(code));
  buffer += sizeof(code);
  sizeBuffer -= sizeof(code);

  // Pack the message field into the buffer
  size_t lengthMessage = strlen(response->message) + 1;
  if (lengthMessage > sizeBuffer) {
    perror("Error: The buffer is too small to hold the serialized message");
    exit(1);
  }
  memcpy(buffer, response->message, lengthMessage);
  buffer += lengthMessage;
  sizeBuffer -= lengthMessage;

  // Pack the from field into the buffer
  size_t lengthFrom = strlen(response->from) + 1;  // include the null terminator
  if (lengthFrom > sizeBuffer) {
    perror("Error: The buffer is too small to hold the serialized from field");
    exit(1);
  }
  memcpy(buffer, response->from, lengthFrom);
}








