#include "../../../include/server/room/create_room_server.h"
#include "../../../include/list/room.h"
#include "../../../include/constants.h"
#include "../../../include/global.h"
#include "../../../include/server/global_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int is_create_room_message(char* message) {
    if (strncmp(message, "/createroom", 8) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void handle_create_room_message(char* message, int socketClient, char* pseudoClient) {
    // Message does not follow the good format
    if(!is_good_format_create_room(message)) {
        char* message = "Create room message have to follow the format /createroom <room_name> <room_description>";
        send_response(socketClient, CREATE_ROOM_ERROR, message, NULL);
        return;
    }

    // Check if the client is not already in a room
    Node* nodeClient = search_element_pseudo(&listClient, pseudoClient);
    if(nodeClient->isInRoom == 1) {
        char* message = "You are already in a room";
        send_response(socketClient, CREATE_ROOM_ERROR, message, NULL);
        return;
    }

    // Get the name of the room to create
    char* roomName = get_name_create_room(message);

    // Get the description of the room to create
    char* roomDescription = get_description_create_room(message);

    // Create the room
    Room* newRoom = insert_first_room(&listRoom, listRoom->countClient+1, roomName, roomDescription, pseudoClient);

    // Check if the room is created
    if(newRoom == NULL) {
        char* message = "An error occurred while creating the room";
        send_response(socketClient, CREATE_ROOM_ERROR, message, NULL);
        return;
    }

    // Send the confirmation to the client
    char* responseCreate = "You created the room";
    send_response(socketClient, CREATE_ROOM_SUCCESS, responseCreate, NULL);

    // Add the client to the room
    char* errorMessage = malloc(sizeof(char) * NB_CHARACTERS);
    if(add_client_to_room(newRoom, socketClient, pseudoClient, errorMessage) == 0) {
        send_response(socketClient, JOIN_ROOM_ERROR, errorMessage, NULL);
        return;
    }

    // Indicate that the client joined the room in the Node structure of the client
    nodeClient->isInRoom = 1;
    nodeClient->roomName = malloc(strlen(roomName) + 1);
    strcpy(nodeClient->roomName, roomName);

    // Send the confirmation to the client
    char* responseJoin = "You joined the room";
    send_response(socketClient, JOIN_ROOM_SUCCESS, responseJoin, NULL);
}


int is_good_format_create_room(char* message) {
    // Check if the message start with "/createroom "
    if(strncmp(message, "/createroom ", 12) != 0) {
        return 0;
    }
    printf("test1\n");

    // Find the end of the room name (the first space after "/createroom ")
    const char* roomNameEnd = strchr(message + 12, ' ');
    if(roomNameEnd == NULL) {
        return 0;
    }
    printf("test2\n");

    // Check if there is at least one character after the room name + ' '
    if(strlen(roomNameEnd) <= 1) {
        return 0;
    }
    printf("test3\n");

    return 1;
}


char* get_name_create_room(char* message) {
    // Find the end of the room name (the first space after "/createroom ")
    const char* roomNameEnd = strchr(message + 12, ' ');

    // Get the length of the room name
    int roomNameLength = roomNameEnd - (message + 12);

    // Allocate memory for the room name
    char* roomName = malloc(sizeof(char) * (roomNameLength + 1));

    // Copy the room name
    strncpy(roomName, message + 12, roomNameLength);

    // Add the end of string character
    roomName[roomNameLength] = '\0';

    return roomName;
}


char* get_description_create_room(char* message) {
    // Find the end of the room name (the first space after "/createroom ")
    const char* roomNameEnd = strchr(message + 12, ' ');

    // Get the length of the room name
    int roomNameLength = roomNameEnd - (message + 12);

    // Get the length of the room description
    int roomDescriptionLength = strlen(message) - 12 - roomNameLength - 1;

    // Allocate memory for the room description
    char* roomDescription = malloc(sizeof(char) * (roomDescriptionLength + 1));

    // Copy the room description
    strncpy(roomDescription, message + 12 + roomNameLength + 1, roomDescriptionLength);

    // Add the end of string character
    roomDescription[roomDescriptionLength] = '\0';

    return roomDescription;
}