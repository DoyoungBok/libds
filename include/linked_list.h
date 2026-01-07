#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

/*
 * LinkedListNode
 * --------------
 * A single node in a singly linked list.
 */
typedef struct LinkedListNode {
    int value;
    struct LinkedListNode *next;
} LinkedListNode;

/*
 * LinkedList
 * ----------
 * Singly linked list storing integers.
 */
typedef struct {
    LinkedListNode *head;
    size_t size;
} LinkedList;

/*
 * LLResult
 * --------
 * Return codes for safe linked list operations.
 */
typedef enum {
    LL_OK = 0,
    LL_ERR_NULL,
    LL_ERR_OOB,
    LL_ERR_EMPTY,
    LL_ERR_ALLOC
} LLResult;


/* ============================================================
 *                   Constructor / Destructor
 * ============================================================ */

/*
 * Creates an empty linked list.
 * Returns NULL if allocation fails.
 */
LinkedList* ll_create(void);

/*
 * Frees all nodes and the list structure itself.
 * Safe to call with NULL.
 */
void ll_destroy(LinkedList *list);


/* ============================================================
 *                   Unsafe Operations
 * ============================================================ */

void ll_push_front(LinkedList *list, int value);
void ll_push_back(LinkedList *list, int value);
int  ll_get(const LinkedList *list, size_t index);
void ll_remove_at(LinkedList *list, size_t index);
size_t ll_size(const LinkedList *list);


/* ============================================================
 *                   Safe Operations
 * ============================================================ */

LLResult ll_push_front_try(LinkedList *list, int value);
LLResult ll_push_back_try(LinkedList *list, int value);
LLResult ll_get_checked(const LinkedList *list, size_t index, int *out_value);
LLResult ll_remove_checked(LinkedList *list, size_t index);

#endif // LINKED_LIST_H
