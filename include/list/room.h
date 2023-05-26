#ifndef ROOM_H
#define ROOM_H

#include "../constants.h"
#include <pthread.h>

typedef struct ClientRoom {
    int socketClient;
    char* pseudoClient;
} ClientRoom;

// Structure of a room to create a list circular doubly linked with head of list
typedef struct Room {
    int number; // number corresponding to count for head of list and number of room for other
	char* name;
	char* description;
    struct ClientRoom clients[MAX_NB_CLIENT];
    struct Room* prev;
    struct Room* next;
} Room;


// rwlock to handle concurrent access to list
pthread_rwlock_t rwlockRoom;

/**
 * Initialize the head of the list
 *
 * @param head The head of the list which was declare before
 *
 * @return void
 */
extern void init_head_room(Room** head);

/**
 * Check if the list is empty
 *
 * @param head The head of the list
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return 1 if list is empty | 0 if list is not empty
 */
extern int is_empty_room(Room** head, int isMutexAccess);

/**
 * Check if the two elements are equal
 *
 * @param element1 The first element
 * @param element2 The second element
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return 1 if elements are equal | 0 if elements are not equal
 */
extern int is_equal_room(Room* room1, Room* room2, int isMutexAccess);

/**
 * Add a room at start of the list
 *
 * @param head The head of the list
 * @param number The number of the room
 * @param name The name of the room
 * @param description The description of the room
 *
 * @return The new room
 */
extern Room* insert_first_room(Room** head, int number, char* name, char* description);

/**
 * Remove the room specified in parameter from the list
 *
 * @param head The head of the list
 * @param room The room to remove
 *
 * @return void
 */
extern void remove_room(Room** head, Room* room);


/**
 * Get the list of rooms in a string
 *
 * @param head The head of the list
 *
 * @return The list of rooms in a string
 */
extern char* get_list_rooms(Room** head);

/**
 * Search a room with the specified number in the list
 *
 * @param head The head of the list
 * @param number The number to search
 *
 * @return The room if found | NULL if not found
 */
extern Room* search_room(Room** head, int number);

/**
 * Display all rooms of the list
 *
 * @param head The head of the list
 *
 * @return void
 */
extern void display_list_room(Room** head);

/**
 * Lock the list for reading
 *
 * @return void
 */
extern void read_lock_room();

/**
 * Lock the list for writing
 *
 * @return void
 */
extern void write_lock_room();

/**
 * Unlock the list
 *
 * @return void
 */
extern void unlock_room();

#endif // ROOM_H