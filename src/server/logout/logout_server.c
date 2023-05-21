#include "../../../include/server/logout/logout_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_logout_message(char* message) {
  if (strncmp(message, "/logout", 7) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_logout_message(char* message, int socketClient, pthread_t threadId) {
  int goodFormat = is_good_format_logout_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Logout message have to follow the format /logout";
    send_response(socketClient, DISCONNECT_BAD_FORMAT, message, NULL);
  }

  // Message follow the good format
  if(goodFormat == 1) {
    remove_client(socketClient, threadId);
  }
}


int is_good_format_logout_message(char* message) {
  // Check if the string starts with "/logout"
  if (strncmp(message, "/logout", 7) != 0) {
    return 0;
  }

  // Check if there is no character after "/logout"
  if(strlen(message + 7) != 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}
