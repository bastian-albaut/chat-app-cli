#include "../../../include/server/room/delete_room_server.h"
#include "../../../include/list/room.h"
#include "../../../include/constants.h"
#include "../../../include/global.h"
#include "../../../include/server/global_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int is_delete_room_message(char* message) {
    if (strncmp(message, "/deleteroom", 8) == 0) {
        return 1;
    } else {
        return 0;
    }
}


void handle_delete_room_message(char* message, int socketClient, char* pseudoClient) {
    // Message does not follow the good format
    if(!is_good_format_delete_room(message)) {
        char* message = "Delete room message have to follow the format /deleteroom <room_name>";
        send_response(socketClient, DELETE_ROOM_ERROR, message, NULL);
        return;
    }

    // Get the name of the room
    char* roomName = get_name_delete_room(message);

    // Check if the room exists
    Room* roomToDelete = search_room(&listRoom, roomName);
    if(roomToDelete == NULL) {
        char* message = "The room does not exist";
        send_response(socketClient, DELETE_ROOM_ERROR, message, NULL);
        return;
    }

    // Check if the client is the owner of the room
    if(strcmp(roomToDelete->author, pseudoClient) != 0) {
        char* message = "You are not the owner of the room";
        send_response(socketClient, DELETE_ROOM_ERROR, message, NULL);
        return;
    }

    // Remove all clients and delete the room
    if(delete_room(&listRoom, roomToDelete) == 0) {
        char* message = "An error occurred while deleting the room";
        send_response(socketClient, DELETE_ROOM_ERROR, message, NULL);
        return;
    }

    // Send the confirmation to the client
    char* messageResponse = "The room is deleted";
    send_response(socketClient, DELETE_ROOM_SUCCESS, messageResponse, NULL);
}


int is_good_format_delete_room(char* message) {
    // Check if the message start with "/deleteroom "
    if(strncmp(message, "/deleteroom ", 12) != 0) {
        return 0;
    }

    // Check if there is a space after "/deleteroom"
    if(message[11] != ' ') {
        return 0;
    }

    // Check if there is no character after "/deleteroom "
    if(strlen(message + 12) == 0) {
        return 0;
    }

    return 1;
}


char* get_name_delete_room(char* message) {
    // Get the content of the message
    char* content = malloc(strlen(message + 12) + 1);
    strcpy(content, message + 12);

    return content;
}