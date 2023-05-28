#include "../../../include/server/room/quit_room_server.h"
#include "../../../include/server/room/enter_room_server.h"
#include "../../../include/list/room.h"
#include "../../../include/constants.h"
#include "../../../include/global.h"
#include "../../../include/server/global_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int is_quit_room_message(char* message) {
    if (strncmp(message, "/quitroom", 9) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void handle_quit_room_message(char* message, int socketClient, char* pseudoClient) {
    // Message does not follow the good format
    if(!is_good_format_quit_room(message)) {
        char* message = "Quit room message have to follow the format /quitroom <room_name>";
        send_response(socketClient, QUIT_ROOM_ERROR, message, NULL);
        return;
    }

    // Get the room name
    char* roomName = get_room_name(message);
    printf("Room name: %s\n", roomName);
    // Check if the room exists 
    Room* roomToQuit = search_room(&listRoom, roomName);
    if(roomToQuit == NULL) {
        char* response = "The room does not exist";
        send_response(socketClient, QUIT_ROOM_ERROR, response, NULL);
        return;
    }

    // If the client is the owner of the room, he can't quit the room
    if(strcmp(roomToQuit->author, pseudoClient) == 0) {
        char* response = "You can't quit the room because you are the owner";
        send_response(socketClient, QUIT_ROOM_ERROR, response, NULL);
        return;
    }

    // Remove the client from the room
    char* errorMessage = malloc(sizeof(char) * NB_CHARACTERS);
    if(remove_client_from_room(roomToQuit, socketClient, pseudoClient, errorMessage, 1) == 0) {
        send_response(socketClient, QUIT_ROOM_ERROR, errorMessage, NULL);
        return;
    }

    // Indicate that the client is not in a room in the Node structure of the client
    Node* nodeClient = search_element_pseudo(&listClient, pseudoClient);
    nodeClient->isInRoom = 0;
    nodeClient->roomName = NULL;

    // Send the confirmation to the client
    char* response = "You successfully quit the room";
    send_response(socketClient, QUIT_ROOM_SUCCESS, response, NULL);
}


int is_good_format_quit_room(char* message) {
    // Check if the string starts with "/quitroom"
    if (strncmp(message, "/quitroom", 9) != 0) {
        return 0;
    }

    // Check if there is a space after "/quitroom"
    if(message[9] != ' ') {
        return 0;
    }

    // Check if there is no character after "/quitroom "
    if(strlen(message + 10) == 0) {
        return 0;
    }

    // The string is in the correct format
    return 1;
}