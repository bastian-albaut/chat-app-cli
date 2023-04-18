#include <pthread.h>

typedef struct Node {
    int number; // number corresponding to count for head
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

// Display all elements of the list
extern void display_list(Node** head);