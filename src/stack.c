#include "stack.h"
#include <stdlib.h>

Stack* stack_create(size_t initial_capacity) {
    Stack *st = malloc(sizeof(Stack));
    if (!st) return NULL;

    st->buf = da_create(initial_capacity);
    if (!st->buf) {
        free(st);
        return NULL;
    }
    return st;
}

void stack_destroy(Stack *st) {
    if (!st) return;
    da_destroy(st->buf);
    free(st);
}

/* ===================== Unsafe API ===================== */

void stack_push(Stack *st, int value) {
    (void)stack_push_try(st, value);
}

int stack_pop(Stack *st) {
    int out = 0;
    (void)stack_pop_checked(st, &out);
    return out;
}

int stack_peek(const Stack *st) {
    int out = 0;
    (void)stack_peek_checked(st, &out);
    return out;
}

size_t stack_size(const Stack *st) {
    if (!st || !st->buf) return 0;
    return da_size(st->buf);
}

/* ===================== Safe API ===================== */

StackResult stack_push_try(Stack *st, int value) {
    if (!st || !st->buf) return STACK_ERR_NULL;

    DAResult r = da_append_try(st->buf, value);
    if (r == DA_OK) return STACK_OK;
    if (r == DA_ERR_ALLOC) return STACK_ERR_ALLOC;
    return STACK_ERR_INTERNAL;
}

StackResult stack_pop_checked(Stack *st, int *out_value) {
    if (!st || !st->buf || !out_value) return STACK_ERR_NULL;
    if (da_size(st->buf) == 0) return STACK_ERR_EMPTY;

    // Top is the last element
    size_t last = da_size(st->buf) - 1;
    int v = 0;

    DAResult gr = da_get_checked(st->buf, last, &v);
    if (gr != DA_OK) return STACK_ERR_INTERNAL;

    DAResult rr = da_remove_checked(st->buf, last);
    if (rr != DA_OK) return STACK_ERR_INTERNAL;

    *out_value = v;
    return STACK_OK;
}

StackResult stack_peek_checked(const Stack *st, int *out_value) {
    if (!st || !st->buf || !out_value) return STACK_ERR_NULL;
    if (da_size(st->buf) == 0) return STACK_ERR_EMPTY;

    size_t last = da_size(st->buf) - 1;
    int v = 0;

    DAResult r = da_get_checked(st->buf, last, &v);
    if (r != DA_OK) return STACK_ERR_INTERNAL;

    *out_value = v;
    return STACK_OK;
}
