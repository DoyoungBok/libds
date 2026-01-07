#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include "dynamic_array.h"

/*
 * Queue
 * -----
 * A FIFO queue implemented on top of DynamicArray.
 * Front of the queue corresponds to index 0.
 *
 * Note: da_remove at index 0 is O(n) due to shifting.
 * This is fine for learning; later we can upgrade to a circular buffer.
 */
typedef struct {
    DynamicArray *buf;
} Queue;

typedef enum {
    QUEUE_OK = 0,
    QUEUE_ERR_NULL,
    QUEUE_ERR_EMPTY,
    QUEUE_ERR_ALLOC,
    QUEUE_ERR_INTERNAL
} QueueResult;

/* Constructor / Destructor */
Queue* queue_create(size_t initial_capacity);
void queue_destroy(Queue *q);

/* Unsafe operations */
void queue_enqueue(Queue *q, int value);
int  queue_dequeue(Queue *q);
int  queue_front(const Queue *q);
size_t queue_size(const Queue *q);

/* Safe operations */
QueueResult queue_enqueue_try(Queue *q, int value);
QueueResult queue_dequeue_checked(Queue *q, int *out_value);
QueueResult queue_front_checked(const Queue *q, int *out_value);

#endif // QUEUE_H
