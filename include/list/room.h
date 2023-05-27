#ifndef ROOM_H
#define ROOM_H

#include "../constants.h"
#include <pthread.h>

// Structure of a client in a room to create a list simply linked without head of list
typedef struct ClientRoom {
    int socketClient;
    char* pseudoClient;
    struct ClientRoom* next;
} ClientRoom;

// Structure of a room to create a list circular doubly linked with head of list
typedef struct Room {
    int number; // number corresponding to count for head of list and id of room for other
	char* name;
	char* description;
    int countClient;
    struct ClientRoom* firstClient;
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
 * Check if the list of room(s) is empty
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
 * Check if the number of rooms is equal to the capacity
 *
 * @param head The head of the list
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return 1 if list is full | 0 if list is not full
 */
extern int is_full_room(Room** head, int isMutexAccess);

/**
 * Check if the number of clients in the room is equal to the capacity
 *
 * @param room The room to check
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return 1 if room is full | 0 if room is not full
 */
extern int is_full_room_list_client(Room* room, int isMutexAccess);

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
 * Search a room with the specified name in the list
 *
 * @param head The head of the list
 * @param name The name of the room to search
 *
 * @return The room if found | NULL if not found
 */
extern Room* search_room(Room** head, char* name);

/**
 * Check if the room specified in parameter is full
 *
 * @param room The room to check
 *
 * @return 1 if room is full | 0 if room is not full
 */
extern int is_room_full(Room* room);

/**
 * Add a client to the room specified in parameter
 *
 * @param room The room to add the client
 * @param socketClient The socket of the client
 * @param pseudoClient The pseudo of the client
 * @param errorMessage The error message to send to the client if an error occurred
 *
 * @return 1 if client is added | 0 if client is not added
 */
extern int add_client_to_room(Room* room, int socketClient, char* pseudoClient, char* errorMessage);

/**
 * Search and return the client in the room specified in parameter
 *
 * @param room The room to search the client
 * @param pseudoClient The pseudo of the client to search
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return The client if found | NULL if not found
 */
extern ClientRoom* search_client_in_room(Room* room, char* pseudoClient, int isMutexAccess);

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