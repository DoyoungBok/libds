#include "linked_list.h"
#include <stdlib.h>

/*
 * Internal helper: creates a new node.
 */
static LinkedListNode* ll_node_create(int value) {
    LinkedListNode *node = malloc(sizeof(LinkedListNode));
    if (!node) return NULL;
    node->value = value;
    node->next = NULL;
    return node;
}

LinkedList* ll_create(void) {
    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) return NULL;

    list->head = NULL;
    list->size = 0;
    return list;
}

void ll_destroy(LinkedList *list) {
    if (!list) return;

    LinkedListNode *curr = list->head;
    while (curr) {
        LinkedListNode *next = curr->next;
        free(curr);
        curr = next;
    }
    free(list);
}

/* ===================== Unsafe API ===================== */

void ll_push_front(LinkedList *list, int value) {
    (void)ll_push_front_try(list, value);
}

void ll_push_back(LinkedList *list, int value) {
    (void)ll_push_back_try(list, value);
}

int ll_get(const LinkedList *list, size_t index) {
    LinkedListNode *curr = list->head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->next;
    }
    return curr->value;
}

void ll_remove_at(LinkedList *list, size_t index) {
    (void)ll_remove_checked(list, index);
}

size_t ll_size(const LinkedList *list) {
    return list ? list->size : 0;
}

/* ===================== Safe API ===================== */

LLResult ll_push_front_try(LinkedList *list, int value) {
    if (!list) return LL_ERR_NULL;

    LinkedListNode *node = ll_node_create(value);
    if (!node) return LL_ERR_ALLOC;

    node->next = list->head;
    list->head = node;
    list->size++;
    return LL_OK;
}

LLResult ll_push_back_try(LinkedList *list, int value) {
    if (!list) return LL_ERR_NULL;

    LinkedListNode *node = ll_node_create(value);
    if (!node) return LL_ERR_ALLOC;

    if (!list->head) {
        list->head = node;
    } else {
        LinkedListNode *curr = list->head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = node;
    }

    list->size++;
    return LL_OK;
}

LLResult ll_get_checked(const LinkedList *list, size_t index, int *out_value) {
    if (!list || !out_value) return LL_ERR_NULL;
    if (index >= list->size) return LL_ERR_OOB;

    LinkedListNode *curr = list->head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->next;
    }

    *out_value = curr->value;
    return LL_OK;
}

LLResult ll_remove_checked(LinkedList *list, size_t index) {
    if (!list) return LL_ERR_NULL;
    if (list->size == 0) return LL_ERR_EMPTY;
    if (index >= list->size) return LL_ERR_OOB;

    if (index == 0) {
        LinkedListNode *old = list->head;
        list->head = old->next;
        free(old);
    } else {
        LinkedListNode *curr = list->head;
        for (size_t i = 0; i < index - 1; i++) {
            curr = curr->next;
        }
        LinkedListNode *old = curr->next;
        curr->next = old->next;
        free(old);
    }

    list->size--;
    return LL_OK;
}
