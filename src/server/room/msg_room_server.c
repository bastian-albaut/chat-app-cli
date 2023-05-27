#include "../../../include/server/room/msg_room_server.h"
#include "../../../include/list/room.h"
#include "../../../include/constants.h"
#include "../../../include/global.h"
#include "../../../include/server/global_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int is_msg_room_message(char* message) {
    if (strncmp(message, "/msgroom", 8) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void handle_msg_room_message(char* message, int socketClient, char* pseudoClient) {
    // Message does not follow the good format
    if(!is_good_format_msg_room(message)) {
        char* message = "Join room message have to follow the format /msgroom <message>";
        send_response(socketClient, MSG_ROOM_ERROR, message, NULL);
        return;
    }

    // Check if the client is in a room
    Node* nodeClient = search_element_pseudo(&listClient, pseudoClient);
    if(nodeClient->isInRoom == 0) {
        char* message = "You are not in a room";
        send_response(socketClient, MSG_ROOM_ERROR, message, NULL);
        return;
    }

    // Get the room of the client
    char* roomName = nodeClient->roomName;
    Room* room = search_room(&listRoom, roomName);

    // Send the message to the other clients in the room
    int result = send_to_other_clients_room(message, room, socketClient, pseudoClient);
    if(result == 1) {
      char* message = "Message send to all clients in the room";
      send_response(socketClient, MSG_ROOM_SUCCESS, message, NULL);
    } else if(result == -1) {
      char* message = "Error while sending message to all clients";
      send_response(socketClient, MSG_ROOM_ERROR, message, NULL);
    } else {
      char* message = "There is no other clients";
      send_response(socketClient, NO_OTHER_USERS, message, NULL);
    }
}


int is_good_format_msg_room(char* message) {
    // Check if the string starts with "/msgroom"
    if (strncmp(message, "/msgroom", 8) != 0) {
        return 0;
    }

    // Check if there is a space after "/msgroom"
    if(message[8] != ' ') {
        return 0;
    }

    // Check if there is no character after "/msgroom "
    if(strlen(message + 9) == 0) {
        return 0;
    }

    // The string is in the correct format
    return 1;
}

int send_to_other_clients_room(char* message, Room* room, int socketClient, char* pseudoTransmitter) {
    // Check if there is other clients in the room
    if(room->countClient == 1) {
        return 0;
    }

    // Get the first client of the room
    ClientRoom* nodeClientToTransmit = room->firstClient;

    // Send the message to all the clients in the room
    while(nodeClientToTransmit != NULL) {
        // Check if the client is not the transmitter
        if(nodeClientToTransmit->socketClient != socketClient) {
            // Send the message to the client
            if(send_response(nodeClientToTransmit->socketClient, MESSAGE_ROOM_REDIRECT, message, pseudoTransmitter) == -1) {
                return -1;
            }
        }

        // Go to the next client
        nodeClientToTransmit = nodeClientToTransmit->next;
    }

    return 1;
}