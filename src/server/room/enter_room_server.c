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

int is_join_room_message(char* message) {
    if (strncmp(message, "/joinroom", 9) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void handle_join_room_message(char* message, int socketClient, char* pseudoClient) {
    // Message does not follow the good format
    if(!is_good_format_join_room(message)) {
        char* message = "Join room message have to follow the format /joinroom <room_name>";
        send_response(socketClient, JOIN_ROOM_ERROR, message, NULL);
        return;
    }

    // Check if the client is not already in a room
    Node* nodeClient = search_element_pseudo(&listClient, pseudoClient);
    if(nodeClient->isInRoom == 1) {
        char* message = "You are already in a room";
        send_response(socketClient, JOIN_ROOM_ERROR, message, NULL);
        return;
    }

    // Get the room name
    char* roomName = get_room_name(message);
    printf("Room name: %s\n", roomName);
    // Check if the room exists 
    Room* roomToJoin = search_room(&listRoom, roomName);
    if(roomToJoin == NULL) {
        char* response = "The room does not exist";
        send_response(socketClient, JOIN_ROOM_ERROR, response, NULL);
        return;
    }

    // Add the client to the room
    char* errorMessage = malloc(sizeof(char) * NB_CHARACTERS);
    if(add_client_to_room(roomToJoin, socketClient, pseudoClient, errorMessage) == 0) {
        send_response(socketClient, JOIN_ROOM_ERROR, errorMessage, NULL);
        return;
    }

    // Indicate that the client joined the room in the Node structure of the client
    nodeClient->isInRoom = 1;

    // Send the confirmation to the client
    char* response = "You joined the room";
    send_response(socketClient, JOIN_ROOM_SUCCESS, response, NULL);
}


int is_good_format_join_room(char* message) {
    // Check if the string starts with "/joinroom"
    if (strncmp(message, "/joinroom", 9) != 0) {
        return 0;
    }

    // Check if there is a space after "/joinroom"
    if(message[9] != ' ') {
        return 0;
    }

    // Check if there is no character after "/joinroom "
    if(strlen(message + 10) == 0) {
        return 0;
    }

    // The string is in the correct format
    return 1;
}


char* get_room_name(char* message) {
    // Get the room name
    char* roomName = malloc(strlen(message + 10) + 1);
    strcpy(roomName, message + 10);

    return roomName;
}