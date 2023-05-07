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

// rwlock to handle concurrent access to list
pthread_rwlock_t rwlock;

extern void init_head(Node** head);
extern int is_empty(Node** head);
extern int is_equal(Node* element1, Node* element2);
extern Node* insert_first(Node** head, int number, char* pseudo, pthread_t thread);
extern void remove_element(Node** head, Node* element);
extern Node* search_element(Node** head, int number);
extern Node* search_element_pseudo(Node** head, char* pseudo);
extern void display_list(Node** head);
extern void read_lock();
extern void write_lock();
extern void unlock();

#endif // LIST_H