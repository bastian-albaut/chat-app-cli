/* List circular doubly linked with head of list */

#include "../include/list/room.h"
#include "../include/global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void init_head_room(Room** head) {
    *head = (Room*) malloc(sizeof(Room));
    (*head)->next = *head;
    (*head)->prev = *head;
    (*head)->number = 0; // Corresponding to count of rooms in list (head is not counted)

    printf("List of room(s) initialization...\n");
    // Initialize rwlockRoom
    if(pthread_rwlock_init(&rwlockRoom, NULL) != 0) {
        perror("pthread_rwlock_init");
        exit(1);
    }
    printf("=> rwlockRoom initialized\n\n");
}


int is_empty_room(Room** head, int isMutexAccess){

    if(isMutexAccess) {
        // Lock list for reading
        read_lock_room();
    }

    int isEmpty = (*head)->next == *head && (*head)->prev == *head;

    if(isMutexAccess) {
        // Unlock list
        unlock_room();
    }

    return isEmpty;
}


int is_equal_room(Room* room1, Room* room2, int isMutexAccess) {

    if(isMutexAccess) {
        // Lock list for reading
        read_lock_room();
    }
    
    int isEqual = room1 == room2;

    if(isMutexAccess) {
        // Unlock list
        unlock_room();
    }

    return isEqual;
}


int is_full_room(Room** head, int isMutexAccess) {
    
    if(isMutexAccess) {
        // Lock list for reading
        read_lock_room();
    }

    int isFull = (*head)->number >= MAX_NB_ROOM;

    if(isMutexAccess) {
        // Unlock list
        unlock_room();
    }

    return isFull;
}


int is_full_room_list_client(Room* room, int isMutexAccess) {
    
    if(isMutexAccess) {
        // Lock list for reading
        read_lock_room();
    }

    int isFull = room->countClient >= MAX_NB_CLIENT;

    if(isMutexAccess) {
        // Unlock list
        unlock_room();
    }

    return isFull;
}


Room* insert_first_room(Room** head, int number, char* name, char* description, char* authorPseudo){
    Room *newRoom = (Room*) malloc(sizeof(Room));
    
    // Lock list for writing
    write_lock_room();

    // Check if list of room is not full
    if(is_full_room(head, 0)) {
        unlock_room();
        return NULL;
    }

    // Set newRoom attributes
    newRoom->number = number;
    newRoom->name = name;
    newRoom->description = description;
    newRoom->author = authorPseudo;
    newRoom->next = (*head)->next;
    newRoom->prev = *head;

    // Adjust reference of head and next element
    newRoom->next->prev = newRoom;
    (*head)->next = newRoom;

    // Increment number of elements in list (count)
    (*head)->number++;

    // Unlock list
    unlock_room();

    return newRoom;
}


void remove_room(Room** head, Room* room) {
    // Check if room is NULL
    if(room == NULL) {
        return;
    }

    // Lock list for writing
    write_lock_room();

    // Check if room is head
    if(is_equal_room(*head, room, 0)) {
        unlock_room();
        return;
    }

    // Check if list is empty
    if(is_empty_room(head, 0)) {
        unlock_room();
        return;
    }

    // Adjust reference of previous and next room
    room->prev->next = room->next;
    room->next->prev = room->prev;

    // Free memory
    free(room);

    // Decrement number of elements in list (count)
    (*head)->number--;

    // Unlock list
    unlock_room();
}

char* get_list_rooms(Room** head) {

    // Lock list for reading
    read_lock_room();

    // Check if list is empty
    if(is_empty_room(head, 0)) {
        unlock_room();
        return NULL;
    }

    // Get list of rooms
    Room *current_element = (*head)->next;
    char* list_rooms = (char*) malloc(sizeof(char) * 1024);
    strcpy(list_rooms, "");
    while(current_element != *head) {
        char* room = (char*) malloc(sizeof(char) * 1024);
        sprintf(room, "%s - %s ", current_element->name, current_element->description);
        char* author = (char*) malloc(sizeof(char) * NB_CHARACTERS);
        sprintf(author, BLUE "(✍️  Author: %s)" GREEN, current_element->author);
        strcat(author, "\n");
        strcat(room, author);
        // Display all clients in room
        ClientRoom *current_client = current_element->firstClient;
        while(current_client != NULL) {
            char* client = (char*) malloc(sizeof(char) * NB_CHARACTERS);
            sprintf(client, BLUE "   👤 %s" GREEN, current_client->pseudoClient);
            strcat(client, "\n");
            strcat(room, client);
            current_client = current_client->next;
        }
            
        strcat(list_rooms, room);
        current_element = current_element->next;
    }

    // Unlock list
    unlock_room();

    return list_rooms;
}


Room* search_room(Room** head, char* name) {
    
    // Lock list for reading
    read_lock_room();

    // Check if list is empty
    if(is_empty_room(head, 0)) {
        unlock_room();
        return NULL;
    }

    // Search room
    Room *element_searched = NULL;
    Room *current_element = (*head)->next;
    while(current_element != *head) {
        if(strcmp(current_element->name, name) == 0) {
            element_searched = current_element;
            break;
        }
        current_element = current_element->next;
    }

    // Unlock list
    unlock_room();

    return element_searched;
}


int delete_room(Room** head, Room* room) {
        
    // Lock list for writing
    write_lock_room();

    // Check if room is NULL
    if(room == NULL) {
        unlock_room();
        return 0;
    }

    // Check if list is empty
    if(is_empty_room(head, 0)) {
        unlock_room();
        return 0;
    }

    // Check if room is head
    if(is_equal_room(*head, room, 0)) {
        unlock_room();
        return 0;
    }

    // Remove all clients from the room
    char* errorMessage = malloc(sizeof(char) * NB_CHARACTERS);
    if(remove_all_clients_from_room(room, errorMessage, 0) == 0) {
        unlock_room();
        return 0;
    }

    // Adjust reference of previous and next room
    room->prev->next = room->next;
    room->next->prev = room->prev;

    // Free memory
    free(room);

    // Decrement number of elements in list (count)
    (*head)->number--;

    // Unlock list
    unlock_room();

    return 1;
}


int remove_all_clients_from_room(Room* room, char* errorMessage, int isMutexAccess) {
        
    // Lock list for writing
    if(isMutexAccess) {
        write_lock_room();
    }

    // Check if room is NULL
    if(room == NULL) {
        unlock_room();
        return 0;
    }

    // Check if list of client in room is empty
    if(room->countClient == 0) {
        unlock_room();
        return 1;
    }

    // Remove all clients from the room
    ClientRoom *current_client = room->firstClient;
    while(current_client != NULL) {
        if(remove_client_from_room(room, current_client->socketClient, current_client->pseudoClient, errorMessage, 0) == 0) {
            unlock_room();
            return 0;
        }
        current_client = current_client->next;
    }

    // Unlock list
    if(isMutexAccess) {
        unlock_room();
    }

    return 1;
}


int add_client_to_room(Room* room, int socketClient, char* pseudoClient, char* errorMessage) {
    
    // Lock list for writing
    write_lock_room();

    // Check if room is NULL
    if(room == NULL) {
        unlock_room();
        return 0;
    }

    // Check if the room is full
    if(is_full_room_list_client(room, 0)) {
        strcpy(errorMessage, "The room is full");
        unlock_room();
        return 0;
    }

    // Check if client is already in room
    ClientRoom* actualClient = search_client_in_room(room, pseudoClient, 0);
    if(actualClient != NULL) {
        strcpy(errorMessage, "You are already in this room");
        unlock_room();
        return 0;
    }

    // Add client to room
    ClientRoom *newClient = (ClientRoom*) malloc(sizeof(ClientRoom));
    newClient->socketClient = socketClient;
    newClient->pseudoClient = pseudoClient;
    newClient->next = room->firstClient;

    // Adjust reference of firstClient
    room->firstClient = newClient;

    // Increment number of client in room (count)
    room->countClient++;

    printf("Client %s added to room %s\n", pseudoClient, room->name);
    printf("Number of client in room %s : %d\n", room->name, room->countClient);
    printf("First client: %s\n", room->firstClient->pseudoClient);

    // Unlock list
    unlock_room();

    return 1;
}


int remove_client_from_room(Room* room, int socketClient, char* pseudoClient, char* errorMessage, int isMutexAccess) {
        
    // Lock list for writing
    if(isMutexAccess) {
        write_lock_room();
    }

    // Check if room is NULL
    if(room == NULL) {
        unlock_room();
        return 0;
    }

    // Check if client is in room
    ClientRoom* actualClient = search_client_in_room(room, pseudoClient, 0);
    if(actualClient == NULL) {
        strcpy(errorMessage, "You are not in this room");
        unlock_room();
        return 0;
    }

    ClientRoom *current_element = room->firstClient;
    ClientRoom *previous_element = NULL;
    while(current_element != NULL) {
        if(current_element->socketClient == socketClient) {
            if(previous_element == NULL) {
                room->firstClient = current_element->next;
            } else {
                previous_element->next = current_element->next;
            }
            break;
        }
        previous_element = current_element;
        current_element = current_element->next;
    }

    // Renitialize isInRoom and roomName attributes of the Node corresponding to the client
    Node* nodeClient = search_element_pseudo(&listClient, pseudoClient);
    nodeClient->isInRoom = 0;
    nodeClient->roomName = NULL;

    // Decrement number of client in room (count)
    room->countClient--;

    printf("Client %s removed from room %s\n", pseudoClient, room->name);
    printf("Number of client in room %s : %d\n", room->name, room->countClient);

    // Unlock list
    if(isMutexAccess) {
        unlock_room();
    }

    return 1;
}


ClientRoom* search_client_in_room(Room* room, char* pseudoClient, int isMutexAccess) {
    
    // Lock list for reading
    if(isMutexAccess) {
        read_lock_room();
    }

    // Check if room is NULL
    if(room == NULL) {
        unlock_room();
        return NULL;
    }

    // Search client
    ClientRoom *element_searched = NULL;
    ClientRoom *current_element = room->firstClient;
    while(current_element != NULL) {
        if(strcmp(current_element->pseudoClient, pseudoClient) == 0) {
            element_searched = current_element;
            break;
        }
        current_element = current_element->next;
    }

    // Unlock list
    if(isMutexAccess) {
        unlock_room();
    }

    return element_searched;
}


void display_list_room(Room** head) {

    // Lock list for reading
    read_lock_room();

    // Check if list is empty
    if(is_empty_room(head, 0)) {
        printf("List of room is empty\n");
        unlock_room();
        return;
    }

    // Display all elements
    Room *current_element = (*head)->next;
    printf("\n==================== List of room(s) ====================\n");
    while(current_element != *head) {
        printf("%s - %s (✍️  Author: %s)\n", current_element->name, current_element->description, current_element->author);
        // Display all clients in room
        ClientRoom *current_client = current_element->firstClient;
        while(current_client != NULL) {
            printf("    %s\n", current_client->pseudoClient);
            current_client = current_client->next;
        }
        current_element = current_element->next;
    }
    printf("======================== End list =======================\n\n");

    // Unlock list
    unlock_room();
}


void read_lock_room() {
    if(pthread_rwlock_rdlock(&rwlockRoom) != 0) {
        perror("pthread_rwlock_rdlock");
        exit(1);
    }
}


void write_lock_room() {
    if(pthread_rwlock_wrlock(&rwlockRoom) != 0) {
        perror("pthread_rwlock_wrlock");
        exit(1);
    }
}


void unlock_room() {
    if(pthread_rwlock_unlock(&rwlockRoom) != 0) {
        perror("pthread_rwlock_unlock");
        exit(1);
    }
}