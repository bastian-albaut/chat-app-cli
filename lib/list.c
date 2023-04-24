
/**** List circular doubly linked with head of list ****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/list.h"

/**** Create a new list ****/
void init_head(Node** head) {
    *head = (Node*) malloc(sizeof(Node));
    (*head)->next = *head;
    (*head)->prev = *head;
    (*head)->number = 0; // Corresponding to count of elements in list (head is not counted)
}

/**** Return 1 if list is empty and 0 else ****/
int is_empty(Node** head){
    return (*head)->next == *head && (*head)->prev == *head;
}

/**** Add an element at start of the list ****/
Node* insert_first(Node** head, int number){
    Node *newHead = (Node*) malloc(sizeof(Node));
    
    // Set newHead attributes
    newHead->number = number;
    newHead->next = (*head)->next;
    newHead->prev = *head;

    // Adjust reference of head and next element
    newHead->next->prev = newHead;
    (*head)->next = newHead;

    // Increment number of elements in list (count)
    (*head)->number++;

    return newHead;
}

/**** Remove an element from the list ****/
void remove_element(Node** head, Node* element) {
    
    if(element == *head) {
        perror("Head is not an element");
        return;
    }

    // Check if list is empty
    if(is_empty(head)) {
        perror("List is empty");
        return;
    }

    // Adjust reference of previous and next element
    element->prev->next = element->next;
    element->next->prev = element->prev;

    // Free memory
    free(element);

    // Decrement number of elements in list (count)
    (*head)->number--;
}

/**** Search an element in the list and return it ****/
Node* search_element(Node** head, int number) {
    // Check if list is empty
    if(is_empty(head)) {
        perror("List is empty");
        return NULL;
    }

    // Search element
    Node *current_element = (*head)->next;
    while(current_element != *head) {
        if(current_element->number == number) {
            return current_element;
        }
        current_element = current_element->next;
    }
    return NULL;
}

/**** Set pseudo of an element ****/
void set_pseudo(Node** head, int socket, char* pseudo) {
    Node* element = search_element(head, socket);
    if(element != NULL) {
        element->pseudo = pseudo;
    }
}

/**** Search an element whith specified pseudo in the list and return 1 if found and 0 else ****/
int search_pseudo(Node** head, char* pseudo) {
    // Check if list is empty
    if(is_empty(head)) {
        perror("List is empty");
        return 0;
    }

    // Search element
    Node *current_element = (*head)->next;
    while(current_element != *head) {
        if(current_element->pseudo != NULL) {
            if(strcmp(current_element->pseudo, pseudo) == 0) {
                return 1;
            }
        }
        current_element = current_element->next;
    }
    return 0;
}

/**** Display all elements of the list ****/
void display_list(Node** head) {

    // Check if list is empty
    if(is_empty(head)) {
        printf("List of Client is empty\n");
        return;
    }

    // Display all elements
    Node *current_element = (*head)->next;
    printf("\n------ List of Clients ------\n");
    while(current_element != *head) {
        if(current_element->pseudo != NULL) {
            printf("%d - %s\n", current_element->number, current_element->pseudo);
        }
        current_element = current_element->next;
    }
    printf("------ End list ------\n\n");
}