#include "queue.h"
#include <stdlib.h>

Queue* queue_create(size_t initial_capacity) {
    Queue *q = malloc(sizeof(Queue));
    if (!q) return NULL;

    q->buf = da_create(initial_capacity);
    if (!q->buf) {
        free(q);
        return NULL;
    }
    return q;
}

void queue_destroy(Queue *q) {
    if (!q) return;
    da_destroy(q->buf);
    free(q);
}

/* ===================== Unsafe API ===================== */

void queue_enqueue(Queue *q, int value) {
    (void)queue_enqueue_try(q, value);
}

int queue_dequeue(Queue *q) {
    int out = 0;
    (void)queue_dequeue_checked(q, &out);
    return out;
}

int queue_front(const Queue *q) {
    int out = 0;
    (void)queue_front_checked(q, &out);
    return out;
}

size_t queue_size(const Queue *q) {
    if (!q || !q->buf) return 0;
    return da_size(q->buf);
}

/* ===================== Safe API ===================== */

QueueResult queue_enqueue_try(Queue *q, int value) {
    if (!q || !q->buf) return QUEUE_ERR_NULL;

    DAResult r = da_append_try(q->buf, value);
    if (r == DA_OK) return QUEUE_OK;
    if (r == DA_ERR_ALLOC) return QUEUE_ERR_ALLOC;
    return QUEUE_ERR_INTERNAL;
}

QueueResult queue_dequeue_checked(Queue *q, int *out_value) {
    if (!q || !q->buf || !out_value) return QUEUE_ERR_NULL;
    if (da_size(q->buf) == 0) return QUEUE_ERR_EMPTY;

    int v = 0;
    DAResult gr = da_get_checked(q->buf, 0, &v);
    if (gr != DA_OK) return QUEUE_ERR_INTERNAL;

    DAResult rr = da_remove_checked(q->buf, 0);
    if (rr != DA_OK) return QUEUE_ERR_INTERNAL;

    *out_value = v;
    return QUEUE_OK;
}

QueueResult queue_front_checked(const Queue *q, int *out_value) {
    if (!q || !q->buf || !out_value) return QUEUE_ERR_NULL;
    if (da_size(q->buf) == 0) return QUEUE_ERR_EMPTY;

    int v = 0;
    DAResult r = da_get_checked(q->buf, 0, &v);
    if (r != DA_OK) return QUEUE_ERR_INTERNAL;

    *out_value = v;
    return QUEUE_OK;
}
