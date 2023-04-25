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
extern int is_empty(Node** head);
extern Node* insert_first(Node** head, int number);
extern void remove_element(Node** head, Node* element);
extern Node* search_element(Node** head, int number);
extern Node* search_element_pseudo(Node** head, char* pseudo);
extern void set_pseudo(Node** head, int socket, char* pseudo);
extern void display_list(Node** head);

#endif // LIST_H