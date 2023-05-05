/* List circular doubly linked with head of list */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/list.h"
#include <pthread.h>


/**
 * Initialize the head of the list
 *
 * @param head The head of the list which was declare before
 *
 * @return void
 */
void init_head(Node** head) {
    *head = (Node*) malloc(sizeof(Node));
    (*head)->next = *head;
    (*head)->prev = *head;
    (*head)->number = 0; // Corresponding to count of elements in list (head is not counted)

    printf("Head of list initialized ");
    // Initialize rwlock
    if(pthread_rwlock_init(&rwlock, NULL) != 0) {
        perror("pthread_rwlock_init");
        exit(1);
    }
    printf("=> rwlock initialized\n");
}


/**
 * Check if the list is empty
 *
 * @param head The head of the list
 *
 * @return 1 if list is empty | 0 if list is not empty
 */
int is_empty(Node** head){

    // Lock list for reading
    read_lock();

    int isEmpty = (*head)->next == *head && (*head)->prev == *head;

    // Unlock list
    unlock();

    return isEmpty;
}

/**
 * Check if the two elements are equal
 *
 * @param element1 The first element
 * @param element2 The second element
 *
 * @return 1 if elements are equal | 0 if elements are not equal
 */
int is_equal(Node* element1, Node* element2) {

    // Lock list for reading
    read_lock();

    int isEqual = element1 == element2;

    // Unlock list
    unlock();

    return isEqual;
}


/**
 * Add an element at start of the list
 *
 * @param head The head of the list
 * @param number The number which will be set in the new element
 *
 * @return The new element
 */
Node* insert_first(Node** head, int number){
    Node *newHead = (Node*) malloc(sizeof(Node));
    
    // Lock list for writing
    write_lock();

    // Set newHead attributes
    newHead->number = number;
    newHead->next = (*head)->next;
    newHead->prev = *head;

    // Adjust reference of head and next element
    newHead->next->prev = newHead;
    (*head)->next = newHead;

    // Increment number of elements in list (count)
    (*head)->number++;

    // Unlock list
    unlock();

    return newHead;
}


/**
 * Remove the element specified in parameter from the list
 *
 * @param head The head of the list
 * @param element The element to remove
 *
 * @return void
 */
void remove_element(Node** head, Node* element) {
    
    if(is_equal(*head, element)) {
        perror("Head is not an element");
        return;
    }

    // Check if list is empty
    if(is_empty(head)) {
        perror("List is empty");
        return;
    }
    
    // Lock list for writing
    write_lock();

    // Adjust reference of previous and next element
    element->prev->next = element->next;
    element->next->prev = element->prev;

    // Free memory
    free(element);

    // Decrement number of elements in list (count)
    (*head)->number--;

    // Unlock list
    unlock();
}


/**
 * Search an element with the specified number in the list
 *
 * @param head The head of the list
 * @param number The number to search
 *
 * @return The element if found | NULL if not found
 */
Node* search_element(Node** head, int number) {
    
    // Check if list is empty
    if(is_empty(head)) {
        perror("List is empty");
        return NULL;
    }

    // Lock list for reading
    read_lock();

    // Search element
    Node *element_searched = NULL;
    Node *current_element = (*head)->next;
    while(current_element != *head) {
        if(current_element->number == number) {
            element_searched = current_element;
            break;
        }
        current_element = current_element->next;
    }

    // Unlock list
    unlock();

    return element_searched;
}


/**
 * Search an element with the specified pseudo in the list
 *
 * @param head The head of the list
 * @param pseudo The pseudo to search
 *
 * @return The element if found | NULL if not found
 */
Node* search_element_pseudo(Node** head, char* pseudo) {
    
    // Check if list is empty
    if(is_empty(head)) {
        perror("List is empty");
        return NULL;
    }  

    // Lock list for reading
    read_lock();

    // Search element
    Node *element_searched = NULL;
    Node *current_element = (*head)->next;
    while(current_element != *head) {
        if(current_element->pseudo != NULL) {
            if(strcmp(current_element->pseudo, pseudo) == 0) {
                element_searched = current_element;
                break;
            }
        }
        current_element = current_element->next;
    }

    // Unlock list
    unlock();

    return element_searched;
}


/**
 * Set pseudo of the element with the specified socket
 *
 * @param head The head of the list
 * @param socket The socket of the element to set pseudo
 * @param pseudo The pseudo to set
 *
 * @return void
 */
void set_pseudo(Node** element, char* pseudo) {

    // Lock list for writing
    write_lock();    

    if(*element != NULL) {
        (*element)->pseudo = pseudo;
    }

    // Unlock list
    unlock();
}


/**
 * Display all elements of the list
 *
 * @param head The head of the list
 *
 * @return void
 */
void display_list(Node** head) {

    // Check if list is empty
    if(is_empty(head)) {
        printf("List of Client is empty\n");
        return;
    }

    // Lock list for reading
    read_lock();

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

    // Unlock list
    unlock();
}


/**
 * Lock the list for reading
 *
 * @return void
 */
void read_lock() {
    if(pthread_rwlock_rdlock(&rwlock) != 0) {
        perror("pthread_rwlock_rdlock");
        exit(1);
    }
}


/**
 * Lock the list for writing
 *
 * @return void
 */
void write_lock() {
    if(pthread_rwlock_wrlock(&rwlock) != 0) {
        perror("pthread_rwlock_wrlock");
        exit(1);
    }
}


/**
 * Unlock the list
 *
 * @return void
 */
void unlock() {
    if(pthread_rwlock_unlock(&rwlock) != 0) {
        perror("pthread_rwlock_unlock");
        exit(1);
    }
}