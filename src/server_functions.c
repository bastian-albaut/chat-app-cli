#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/server_functions.h"
#include "../include/list.h"
#include "../include/constants.h"
#include "../include/global.h"


/**
 * Function thread to manage each client
 *
 * @param args An instance of ThreadArgs structure with socketClient and listClient
 *
 * @return void
 */
void* thread_client(void* args) {
  ThreadArgs* data = (ThreadArgs*) args;
  int socketClient = data->socketClient;
  Node* listClient = data->listClient;

  // Get pseudo of the client 
  char* pseudo = get_pseudo(socketClient);

  // Set pseudo of client in the list 
  if(pseudo != NULL) {
    set_pseudo(&listClient, socketClient, pseudo);
    printf("Client %s Connected !\n", pseudo);
    display_list(&listClient);
  
    // Loop to receive message and handle messages from client
    int nbByteRead = 1;
    while(nbByteRead != 0 && nbByteRead != -1) {

      // Receive message from client 
      char message[NB_CHARACTERS];
      nbByteRead = recv_message(socketClient, message);

      if(nbByteRead == 0) {
        remove_client(socketClient);
      } else {
        printf("Message receive: %s\n", message);

        int handlePrivateMessage = handle_private_message(message, socketClient);
        if(handlePrivateMessage == 0) {
          // Send message to other clients
          send_to_other_clients(listClient, socketClient, message);
        }
      }
    }
  }

  pthread_exit(0);
}


/**
 * Send private message to a client. OR send an error message if the private message
 * is not in the correct format or if the pseudo of the client does not exist. OR 
 * do nothing if the message is not a private message
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 *
 * @return 1 if the message is a private message | 0 if the message is not a private message
 */
int handle_private_message(char* message, int socketClient) {
  int privateMessage = is_private_message(message);

  // Message does not follow the good format
  if(privateMessage == -1) {
    char* response = "Private message have to follow the format /mp <user> <message>";
    send_message(socketClient, response, NULL);
    return 1;
  } 
  
  // Message is a private message
  if(privateMessage == 1) {
    // Get pseudo of the private message
    char* pseudo = get_pseudo_private_message(message);

    // Get content of the private message
    char* messagePrivate = get_content_private_message(message);

    // Send private message
    if(send_private_message(listClient, pseudo, messagePrivate) == -1) {
      char* response = "There is no user with this pseudo";
      send_message(socketClient, response, NULL);
    }
  }
}

/**
 * Detect if the message corresponding to the format /mp <user> <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is a private message | 0 if the message is not a private message | -1 if the message is private but not in the correct format
 */
int is_private_message(char* message) {
  // Check if the string starts with "/mp "
  if (strncmp(message, "/mp ", 4) != 0) {
    return 0;
  }

  // Find the end of the user name (the first space after "/mp ")
  const char* user_end = strchr(message + 4, ' ');
  if (user_end == NULL) {
    return -1;
  }

  // Check if there is at least one character after the user name
  if (strlen(user_end + 1) == 0) {
    return -1;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Get the destination pseudo of the private message
 *
 * @param message The string to split to get the pseudo
 *
 * @return The pseudo of the private message
 */
char* get_pseudo_private_message(char* message) {
  char* pseudo = malloc(NB_CHARACTERS_PSEUDO * sizeof(char));
  int i = 0;
  int j = 4;
  while(message[j] != ' ') {
    pseudo[i] = message[j];
    i++;
    j++;
  }
  pseudo[i] = '\0';

  return pseudo;
}


/**
 * Get content of the private message
 *
 * @param message The string to split to get the content
 *
 * @return The content of the private message
 */
char* get_content_private_message(char* message) {
  char* content = malloc(NB_CHARACTERS * sizeof(char));
  int i = 0;
  int j = 0;
  while(message[j] != ' ') {
    j++;
  }
  j++;
  while(message[j] != ' ') {
    j++;
  }
  j++;
  while(message[j] != '\0') {
    content[i] = message[j];
    i++;
    j++;
  }
  content[i] = '\0';

  return content;
}


/**
 * Send private message to the client with the pseudo specified in parameter
 *
 * @param head The head of the list of clients
 * @param pseudo The pseudo of the client to send the message
 * @param message The message to send
 *
 * @return 1 if the message is send | -1 if the message is not send
 */
int send_private_message(Node* head, char* pseudo, char* message) {
  Node* elementToSend = search_element_pseudo(&head, pseudo);
  if(elementToSend != NULL) {
    send_message(elementToSend->number, message, NULL);
    printf("Message send to the client %s", pseudo);
    return 1;
  } else {
    return -1;
  }
}


/**
 * Send message to all clients whithout client corresponding to socketClient
 *
 * @param head The head of the list of clients
 * @param socketClient The socket of the client who send the message
 * @param message The message to send
 *
 * @return void
 */
void send_to_other_clients(Node* head, int socketClient, char* message) {
  Node* currentClient = head->next;
  while(currentClient != head) {
    if(currentClient->number != socketClient && currentClient->pseudo != NULL) {
      send_message(currentClient->number, message, NULL);
    }
    currentClient = currentClient->next;
  }
  printf("Message send to other clients\n");
}


/**
 * Get pseudo of the client and check if it is already used
 *
 * @param socketClient The socket of the client which want to get a pseudo
 *
 * @return The pseudo of the client
 */
char* get_pseudo(int socketClient) {

  char* pseudo = malloc(NB_CHARACTERS_PSEUDO * sizeof(char));
  char* response;
  int pseudoAlreadyUsed = 1;

  while(pseudoAlreadyUsed) {
    
    int nbByteRead = recv_message(socketClient, pseudo);
    if(nbByteRead == 0) {
      remove_client(socketClient);
      return NULL;
    }

    // Check if pseudo is already used
    if(search_element_pseudo(&listClient, pseudo) != NULL) {
      response = "ERROR";
    } else {
      response = "SUCCESS";

      // Break loop
      pseudoAlreadyUsed = 0;
    }

    send_message(socketClient, response, NULL);
  }

  return pseudo;
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
  close_all_clients();
  printf("  =>  All client threads/sockets are closed\n");
  close_socket(socketServer);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF SERVER ==========\n");
  exit(0);
}


/**
 * Close all clients thread/sockets in the list
 *
 * @return void
 */
void close_all_clients() {
  Node* currentClient = listClient->next;
  while(currentClient != listClient) {

    // Close thread
    pthread_cancel(currentClient->thread);

    // Close socket
    close_socket(currentClient->number);

    currentClient = currentClient->next;
  }
}


/**
 * Remove the client from the list and close the socket corresponding
 *
 * @param socketClient The socket of the client to remove
 *
 * @return void
 */
void remove_client(int socketClient) {
  remove_element(&listClient, search_element(&listClient, socketClient));
  close_socket(socketClient);
  printf("Client %d disconnected\n", socketClient);
  display_list(&listClient);
}