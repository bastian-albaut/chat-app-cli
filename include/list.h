#ifndef LIST_H
#define LIST_H

#include <pthread.h>

typedef struct Node {
    int number; // number corresponding to count for head
    char* pseudo;
    pthread_t thread;
    struct Node *next;
    struct Node *prev;
} Node;

extern void init_head(Node** head);

// Return 1 if list is empty and 0 else
extern int is_empty(Node** head);

// Add an element at start of the list
extern Node* insert_first(Node** head, int number);

// Remove an element from the list
extern void remove_element(Node** head, Node* element);

// Search an element in the list and return it 
extern Node* search_element(Node** head, int number);

/**** Search an element with pseudo in the list and return it ****/
extern Node* search_element_pseudo(Node** head, char* pseudo);

// Set pseudo of an element
extern void set_pseudo(Node** head, int socket, char* pseudo);

// Search an element whith specified pseudo in the list and return 1 if found and 0 else
extern int search_pseudo(Node** head, char* pseudo);

// Display all elements of the list
extern void display_list(Node** head);

#endif // LIST_H