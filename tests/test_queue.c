#include <assert.h>
#include <stdio.h>
#include "queue.h"

static void test_queue_basic(void) {
    Queue *q = queue_create(2);
    assert(q != NULL);
    assert(queue_size(q) == 0);

    queue_enqueue(q, 10);
    queue_enqueue(q, 20);
    queue_enqueue(q, 30);

    assert(queue_size(q) == 3);
    assert(queue_front(q) == 10);

    assert(queue_dequeue(q) == 10);
    assert(queue_dequeue(q) == 20);
    assert(queue_dequeue(q) == 30);

    queue_destroy(q);
}

static void test_queue_safe_api(void) {
    Queue *q = queue_create(1);
    assert(q != NULL);

    int out = 0;
    assert(queue_dequeue_checked(q, &out) == QUEUE_ERR_EMPTY);
    assert(queue_front_checked(q, &out) == QUEUE_ERR_EMPTY);

    assert(queue_enqueue_try(q, 5) == QUEUE_OK);
    assert(queue_front_checked(q, &out) == QUEUE_OK);
    assert(out == 5);

    assert(queue_dequeue_checked(q, &out) == QUEUE_OK);
    assert(out == 5);
    assert(queue_dequeue_checked(q, &out) == QUEUE_ERR_EMPTY);

    queue_destroy(q);
}

int main(void) {
    test_queue_basic();
    test_queue_safe_api();
    printf("All Queue tests passed!\n");
    return 0;
}
