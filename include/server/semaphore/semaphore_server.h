#ifndef SEMAPHORE_SERVER_H
#define SEMAPHORE_SERVER_H

// Semaphore to handle capacity of clients
extern int idSemaphore;

// Structure for the semaphore
union semun{
    int val;
    struct semid_ds * buff;
    short * array;
};

/**
 * Initialize the semaphore for handle capacity of clients
 *
 * @return void
 */
extern void init_semaphore_server();

/**
 * Set the capacity of the semaphore in global variables
 *
 * @param capacity The capacity to set
 *
 * @return void
 */
extern void set_capacity_semaphore(int capacity);

/**
 * Deletion of the semaphore in global variables
 *
 * @return void
 */
extern void deletion_semaphore();

/**
 * Increment the semaphore specified in parameter to leave place in it
 *
 * @return void
 */
extern void leave_place_semaphore();

/**
 * Display the remaining places in the semaphore
 *
 * @return void
 */
extern void display_remaining_places_semaphore();

#endif // SEMAPHORE_SERVER_H