/* List circular doubly linked with head of list */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/list/room.h"
#include <pthread.h>

void init_head_room(Room** head) {
    *head = (Room*) malloc(sizeof(Room));
    (*head)->next = *head;
    (*head)->prev = *head;
    (*head)->number = 0; // Corresponding to count of elements in list (head is not counted)

    printf("Head of list initialized ");
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


Room* insert_first_room(Room** head, int number, char* name, char* description){
    Room *newHead = (Room*) malloc(sizeof(Room));
    
    // Lock list for writing
    write_lock_room();

    // Set newHead attributes
    newHead->number = number;
    newHead->name = name;
    newHead->description = description;
    newHead->next = (*head)->next;
    newHead->prev = *head;

    // Adjust reference of head and next element
    newHead->next->prev = newHead;
    (*head)->next = newHead;

    // Increment number of elements in list (count)
    (*head)->number++;

    // Unlock list
    unlock_room();

    return newHead;
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


Room* search_room(Room** head, int number) {
    
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
        if(current_element->number == number) {
            element_searched = current_element;
            break;
        }
        current_element = current_element->next;
    }

    // Unlock list
    unlock_room();

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
    printf("\n------ List of room(s) ------\n");
    while(current_element != *head) {
        printf("%s - %s\n", current_element->name, current_element->description);
        current_element = current_element->next;
    }
    printf("------ End list ------\n\n");

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