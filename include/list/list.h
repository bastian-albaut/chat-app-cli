#ifndef LIST_H
#define LIST_H

#include <pthread.h>

// Structure of a node to create a list circular doubly linked with head of list
typedef struct Node {
    int number; // number corresponding to count for head
    char* pseudo;
    pthread_t thread;
    struct Node *next;
    struct Node *prev;
} Node;

// rwlock to handle concurrent access to list
pthread_rwlock_t rwlock;

/**
 * Initialize the head of the list
 *
 * @param head The head of the list which was declare before
 *
 * @return void
 */
extern void init_head(Node** head);

/**
 * Check if the list is empty
 *
 * @param head The head of the list
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return 1 if list is empty | 0 if list is not empty
 */
extern int is_empty(Node** head, int isMutexAccess);

/**
 * Check if the two elements are equal
 *
 * @param element1 The first element
 * @param element2 The second element
 * @param isMutexAccess 1 if mutex access is needed | 0 if mutex access is not needed
 *
 * @return 1 if elements are equal | 0 if elements are not equal
 */
extern int is_equal(Node* element1, Node* element2, int isMutexAccess);

/**
 * Add an element at start of the list
 *
 * @param head The head of the list
 * @param number The number which will be set in the new element
 * @param pseudo The pseudo which will be set in the new element
 * @param thread The thread which will be set in the new element
 *
 * @return The new element
 */
extern Node* insert_first(Node** head, int number, char* pseudo, pthread_t thread);

/**
 * Remove the element specified in parameter from the list
 *
 * @param head The head of the list
 * @param element The element to remove
 *
 * @return void
 */
extern void remove_element(Node** head, Node* element);

/**
 * Search an element with the specified number in the list
 *
 * @param head The head of the list
 * @param number The number to search
 *
 * @return The element if found | NULL if not found
 */
extern Node* search_element(Node** head, int number);

/**
 * Search an element with the specified pseudo in the list
 *
 * @param head The head of the list
 * @param pseudo The pseudo to search
 *
 * @return The element if found | NULL if not found
 */
extern Node* search_element_pseudo(Node** head, char* pseudo);

/**
 * Display all elements of the list
 *
 * @param head The head of the list
 *
 * @return void
 */
extern void display_list(Node** head);

/**
 * Lock the list for reading
 *
 * @return void
 */
extern void read_lock();

/**
 * Lock the list for writing
 *
 * @return void
 */
extern void write_lock();

/**
 * Unlock the list
 *
 * @return void
 */
extern void unlock();

#endif // LIST_H