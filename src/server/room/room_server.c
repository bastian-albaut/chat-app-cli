#include "../../../include/server/room/room_server.h"
#include "../../../include/list/room.h"
#include "../../../include/constants.h"
#include "../../../include/global.h"
#include "../../../include/server/global_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int is_list_rooms_message(char* message) {
  if (strncmp(message, "/listrooms", 11) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_list_rooms_message(char* message, int socketClient) {
  // Message does not follow the good format
  if(!is_good_format_list_rooms(message)) {
    char* message = "List rooms message have to follow the format /listrooms";
    send_response(socketClient, LIST_ROOM_ERROR, message, NULL);
    return;
  }

  // Get the list of rooms
  char* listRooms = get_list_rooms(&listRoom);

  // Send the list of rooms to the client
  send_response(socketClient, LIST_ROOM_SUCCESS, listRooms, NULL);
}


int is_good_format_list_rooms(char* message) {
  // Check if the string starts with "/listrooms"
  if (strncmp(message, "/listrooms", 11) != 0) {
    return 0;
  }

  // Check if there is no character after "/listrooms"
  if(strlen(message + 11) != 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}