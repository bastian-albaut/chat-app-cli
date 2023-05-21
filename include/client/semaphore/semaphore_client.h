#ifndef SEMAPHORE_CLIENT_H
#define SEMAPHORE_CLIENT_H

/**
 * Initialize the semaphore for handle capacity of clients
 *
 * @return void
 */
extern void init_semaphore_client();

/**
 * Decrement the semaphore in global variables to take place in it
 *
 * @return void
 */
extern void take_place_semaphore();

#endif // SEMAPHORE_CLIENT_H