#include "../../../include/server/message/global_message_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include "../../../include/list/list.h"
#include "../../../include/global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int is_global_message(char* message) {
  if (strncmp(message, "/all", 4) == 0) {
    return 1;
  } else {
    return 0;
  }
}


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
      send_response(socketClient, GLOBAL_MESSAGE_SEND, message, NULL);
    } else if(code == -1) {
      char* message = "Error while sending message to all clients";
      send_response(socketClient, GLOBAL_MESSAGE_ERROR, message, NULL);
    } else {
      char* message = "There is no other clients";
      send_response(socketClient, NO_OTHER_USERS, message, NULL);
    }
  }
}


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