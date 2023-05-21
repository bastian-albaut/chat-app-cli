#include "../../../include/server/message/private_message_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include "../../../include/list/list.h"
#include "../../../include/global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int is_private_message(char* message) {
  if (strncmp(message, "/mp", 3) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_private_message(char* message, int socketClient, char* pseudoTransmitter) {
  int goodFormat = is_good_format_private_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Private message have to follow the format /mp <user> <message>";
    send_response(socketClient, PRIVATE_MESSAGE_BAD_FORMAT, message, NULL);
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
      send_response(socketClient, PRIVATE_MESSAGE_USER_NOT_FOUND, message, NULL);
    } else {
      char* message = "Private message send";
      send_response(socketClient, PRIVATE_MESSAGE_SEND, message, NULL);
    }
  }
}


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


char* get_pseudo_private_message(char* message) {
  char* pseudo = malloc(NB_CHARACTERS_PSEUDO_MAX * sizeof(char));
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