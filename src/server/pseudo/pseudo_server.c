#include "../../../include/server/pseudo/pseudo_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include "../../../include/list/list.h"
#include "../../../include/global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int handle_pseudo_client(int socketClient, char* pseudo) {
  // Get pseudo of the client 
  char* pseudoGet = get_pseudo(socketClient);
  
  if(pseudoGet == NULL) {
    return -1;
  }

  // Set pseudo of client in the list 
  if(pseudoGet != NULL) {
    strcpy(pseudo, pseudoGet);
    return 1;
  }
}


char* get_pseudo(int socketClient) {

  char* pseudo = malloc(NB_CHARACTERS_PSEUDO_MAX * sizeof(char));
  int code;
  int pseudoAlreadyUsed = 1;
  int pseudoGoodFormat = 0;

  while(pseudoAlreadyUsed ) {
    
    int nbByteRead = recv_message(socketClient, pseudo);
    if(nbByteRead == 0) {
      printf("Client disconnected during pseudo step\n");
      return NULL;
    }

    // Check if pseudo is in the good format
    char* responseMessage = malloc(NB_CHARACTERS * sizeof(char));
    if(!check_format_pseudo(pseudo, responseMessage)) {
      send_response(socketClient, PSEUDO_BAD_FORMAT, responseMessage, NULL);
      continue;
    } else {
      pseudoGoodFormat = 1;
    }

    // Check if pseudo is already used
    if(search_element_pseudo(&listClient, pseudo) != NULL) {
      char* message = "This pseudo is already used";
      send_response(socketClient, PSEUDO_ALREADY_USED, message, NULL);
    } else {
      pseudoAlreadyUsed = 0;
    }
  }

  // Send response to client
  char* message = "This pseudo is accepted";
  send_response(socketClient, PSEUDO_ACCEPTED, message, NULL);

  return pseudo;
}


int check_format_pseudo(char* pseudo, char* responseMessage) {
  // Check if the pseudo is not too long
  if(strlen(pseudo) > NB_CHARACTERS_PSEUDO_MAX) {
    sprintf(responseMessage, "Your pseudo is too long(>%d characters)", NB_CHARACTERS_PSEUDO_MAX);
    return 0;
  }

  // Check if the pseudo is not too short
  if(strlen(pseudo) < NB_CHARACTERS_PSEUDO_MIN) {
    sprintf(responseMessage, "Your pseudo is too short (<%d characters)", NB_CHARACTERS_PSEUDO_MIN);
    return 0;
  }

  // Check pseudo does not contain space
  if(strchr(pseudo, ' ') != NULL) {
    strcpy(responseMessage, "Your pseudo cannot contain space");
    return 0;
  }

  return 1;
}