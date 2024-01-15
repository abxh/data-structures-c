#include <stdlib.h>
#include <stdbool.h>

#define Q_VALUE int

typedef struct q_element {
    Q_VALUE value;
    struct q_element *next;
} q_element;

typedef struct {
    q_element *front;
    q_element *back;
} queue;

/* Create a new FIFO queue. Can return NULL and it must be checked manually. */
queue *q_new();

/* Return if the queue is empty. Queue pointer cannot be NULL.*/
bool q_empty(queue *queue_p);

/* 
   Peek at the next value to be dequeued. Queue pointer cannot be NULL.
   Must check if queue is empty beforehand.
*/
Q_VALUE q_peek(queue *queue_p);

/* 
   Enqueue a value onto the queue. Returns if memory was able to be allocated
   for the value. Queue pointer cannot be NULL.
*/
bool q_enqueue(queue* queue_p, Q_VALUE value); 

/* 
   Dequeue a value from the queue and return it. Returns if memory was able to
   be allocated for the value. Queue pointer cannot be NULL.
*/
Q_VALUE q_dequeue(queue* queue_p); 

/* Free the queue from the heap. Queue pointer cannot be NULL.*/
void q_free(queue *queue_p); 
