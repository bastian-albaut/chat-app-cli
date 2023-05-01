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
  char* pseudo = malloc(NB_CHARACTERS_PSEUDO * sizeof(char));
  if(handle_pseudo_client(socketClient, pseudo) == -1) {
    remove_client(socketClient);
    pthread_exit(0);
  }

  // Loop to receive message and handle messages from client
  int nbByteRead = 1;
  while(nbByteRead != 0 && nbByteRead != -1) {

    // Receive message from client 
    char message[NB_CHARACTERS];
    nbByteRead = recv_message(socketClient, message);

    if(nbByteRead == 0) {
      remove_client(socketClient);
      pthread_exit(0);
    } 
    
    printf("Message receive: %s\n", message);

    handle_message(message, socketClient, pseudo);
  }

  pthread_exit(0);
}

/**
 * Handle the pseudo of the client
 *
 * @param socketClient The socket of the client which want to get a pseudo
 *
 * @return 1 if handle pseudo work correctly | -1 if not
 */
int handle_pseudo_client(int socketClient, char* pseudo) {
  // Get pseudo of the client 
  char* pseudoGet = get_pseudo(socketClient);
  
  // Set pseudo of client in the list 
  if(pseudoGet != NULL) {
    strcpy(pseudo, pseudoGet);
    set_pseudo(&listClient, socketClient, pseudo);
    printf("Client %s Connected !\n", pseudo);
    display_list(&listClient);
    return 1;
  } else {
    return -1;
  }
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
  int code;
  int pseudoAlreadyUsed = 1;

  while(pseudoAlreadyUsed) {
    
    int nbByteRead = recv_message(socketClient, pseudo);
    if(nbByteRead == 0) {
      printf("Client disconnected during pseudo step\n");
      return NULL;
    }

    // Check if pseudo is already used
    if(search_element_pseudo(&listClient, pseudo) != NULL) {
      code = PSEUDO_ALREADY_USED;
      char* message = "This pseudo is already used";
      send_response(socketClient, code, message, NULL);
    } else {
      code = PSEUDO_ACCEPTED;
      char* message = "This pseudo is accepted";
      send_response(socketClient, code, message, NULL);

      // Break loop
      pseudoAlreadyUsed = 0;
    }
  }

  return pseudo;
}


/**
 * Handle the message of the client
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 * @param pseudo The pseudo of the client who send the message
 *
 * @return void
 */
void handle_message(char* message, int socketClient, char* pseudo) {
  if(is_special_command(message)) {
    if(is_global_message(message)) {
      handle_global_message(message, socketClient, pseudo);
      return;
    }

    if(is_private_message(message)) {
      handle_private_message(message, socketClient, pseudo);
      return;
    }
  }
  char* responseMessage = "Command not found";
  send_response(socketClient, COMMAND_NOT_FOUND, responseMessage, NULL);
}


/**
 * Detect if the message corresponding to a special command (start with "/")
 *
 * @param param message The string to check
 *
 * @return 1 if the message is a special command | 0 if the message is not a special command
 */
char is_special_command(char* message) {
  if (strncmp(message, "/", 1) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a private message (start with "/mp")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a private message | 0 if the message is not a private message
 */
int is_private_message(char* message) {
  if (strncmp(message, "/mp", 3) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a global message (start with "/all")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a global message | 0 if the message is not a global message
 */
int is_global_message(char* message) {
  if (strncmp(message, "/all", 4) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Send private message to a client. OR send an error message if the private message
 * is not in the correct format or if the pseudo of the client does not exist. OR 
 * do nothing if the message is not a private message
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 *
 * @return void
 */
void handle_private_message(char* message, int socketClient, char* pseudoTransmitter) {
  int goodFormat = is_good_format_private_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Private message have to follow the format /mp <user> <message>";
    send_response(socketClient, PRIVATE_MESSAGE_NOT_CORRESPONDING, message, NULL);
  } 
  
  // Message follow the good format
  if(goodFormat == 1) {
    // Get pseudo of the private message
    char* pseudo = get_pseudo_private_message(message);

    // Get content of the private message
    char* messagePrivate = get_content_private_message(message);

    // Send private message
    int code = send_private_message(listClient, pseudo, messagePrivate, pseudoTransmitter);

    // Send response to the client who send the private message
    if(code == -1) {
      char* message = "There is no user with this pseudo";
      send_response(socketClient, PRIVATE_MESSAGE_USER_NOT_EXIST, message, NULL);
    } else {
      char* message = "Private message send";
      send_response(socketClient, MESSAGE_PRIVATE_SEND, message, NULL);
    }
  }
}

/**
 * Detect if the message corresponding to the format /mp <user> <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_private_message(char* message) {
  // Check if the string starts with "/mp "
  if (strncmp(message, "/mp ", 4) != 0) {
    return 0;
  }

  // Find the end of the user name (the first space after "/mp ")
  const char* user_end = strchr(message + 4, ' ');
  if (user_end == NULL) {
    return 0;
  }

  // Check if there is at least one character after the user name
  if (strlen(user_end + 1) == 0) {
    return 0;
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
int send_private_message(Node* head, char* pseudo, char* message, char* pseudoTransmitter) {
  Node* elementToSend = search_element_pseudo(&head, pseudo);
  if(elementToSend != NULL) {
    send_response(elementToSend->number, MESSAGE_PRIVATE_REDIRECT, message, pseudoTransmitter);
    printf("Message send to the client %s\n", pseudo);
    return 1;
  } else {
    return -1;
  }
}


/**
 * Handle global message. Send the message to all clients.
 *
 * @param head The head of the list of clients
 * @param socketClient The socket of the client who send the message
 * @param message The message to send
 *
 * @return void
 */
void handle_global_message(char* message, int socketClient, char* pseudoTransmitter) {
  int goodFormat = is_good_format_global_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Global message have to follow the format /all <message>";
    send_response(socketClient, GLOBAL_MESSAGE_ERROR, message, NULL);
  } 
  
  // Message follow the good format
  if(goodFormat == 1) {

    // Get content of the global message
    char* globalMessage = get_content_global_message(message);

    // Send message to other clients
    int code = send_to_other_clients(globalMessage, socketClient, pseudoTransmitter);
    if(code == 1) {
      char* message = "Message send to all clients";
      send_response(socketClient, MESSAGE_GLOBAL_SEND, message, NULL);
    } else if(code == -1) {
      char* message = "Error while sending message to all clients";
      send_response(socketClient, GLOBAL_MESSAGE_ERROR, message, NULL);
    } else {
      char* message = "There is no other clients";
      send_response(socketClient, NO_OTHER_USERS, message, NULL);
    }
  }
}


/**
 * Detect if the message corresponding to the format /all <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_global_message(char* message) {
  // Check if the string starts with "/all "
  if (strncmp(message, "/all ", 5) != 0) {
    return 0;
  }

  // Check if there is at least one character after "/all "
  if (strlen(message + 5) == 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Get content of the global message
 *
 * @param message The string to split to get the content
 *
 * @return The content of the global message
 */
char* get_content_global_message(char* message) {
  char* content = malloc(NB_CHARACTERS * sizeof(char));
  int i = 0;
  int j = 5;
  while(message[j] != '\0') {
    content[i] = message[j];
    i++;
    j++;
  }
  content[i] = '\0';

  return content;
}


/**
 * Send message to all clients whithout client corresponding to socketClient
 *
 * @param message The message to send
 * @param socketClient The socket of the client who send the message
 * @param pseudoTransmitter The pseudo of the client who send the message
 *
 * @return 1 if all messages are send | -1 if at least one message is not send | 0 if there is no other clients
 */
int send_to_other_clients(char* message, int socketClient, char* pseudoTransmitter) {
  Node* currentClient = listClient->next;
  int errorCatch = 0;
  int countSend = 0;
  while(currentClient != listClient && errorCatch == 0) {
    if(currentClient->number != socketClient && currentClient->pseudo != NULL) {
      if(send_response(currentClient->number, MESSAGE_GLOBAL_REDIRECT, message, pseudoTransmitter) == -1) {
        errorCatch = 1;
      } else {
        countSend += 1;
      }
    }
    currentClient = currentClient->next;
  }

  // If at least one message is not send
  if(errorCatch == 1) {
    perror("Error: Sending message to other clients");
    return -1;
  } 

  // If there is no other clients
  if(countSend == 0) {
    return 0;
  }

  // If all messages are send
  printf("Message send to other clients\n");
  return 1;

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


/**
 * Receive a message from the client with the socket specified in parameter
 *
 * @param socket The socket of the client to recv the message
 * @param message The message to store the received message
 *
 * @return The number of bytes received
 */
int recv_message(int socketClient, char* message) {
  int nbByteRead = recv(socketClient, message, NB_CHARACTERS, 0);
  if(nbByteRead == -1) {
    perror("Error: Receiving the message");
    exit(1);
  }
  return nbByteRead;
}


/**
 * Send an informative, a success, a redirection or an error response of the client with the socket specified in parameter
 *
 * @param socketClient The socket of the client to send the response
 * @param code The code of the response
 * @param message The message of the response
 * @param pseudo The pseudo of the client who send the message (If the response is a redirection)
 *
 * @return 1 if the response is send | -1 if the response is not send
 */
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

/**
 * Serialize all the fields of the response into a buffer specified in parameter
 *
 * @param response The response to serialize
 * @param buffer The buffer to store the serialized response
 * @param sizeBuffer The length of the buffer
 *
 * @return void
 */
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