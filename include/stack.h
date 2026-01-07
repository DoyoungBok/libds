#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include "dynamic_array.h"

/*
 * Stack
 * -----
 * A LIFO stack implemented on top of DynamicArray.
 * Top of the stack corresponds to the end of the dynamic array.
 */
typedef struct {
    DynamicArray *buf; // underlying storage
} Stack;

/*
 * StackResult
 * -----------
 * Return codes for safe stack operations.
 */
typedef enum {
    STACK_OK = 0,
    STACK_ERR_NULL,
    STACK_ERR_EMPTY,
    STACK_ERR_ALLOC,
    STACK_ERR_INTERNAL
} StackResult;

/* Constructor / Destructor */
Stack* stack_create(size_t initial_capacity);
void stack_destroy(Stack *st);

/* Unsafe operations (no error checking) */
void stack_push(Stack *st, int value);
int  stack_pop(Stack *st);
int  stack_peek(const Stack *st);
size_t stack_size(const Stack *st);

/* Safe operations */
StackResult stack_push_try(Stack *st, int value);
StackResult stack_pop_checked(Stack *st, int *out_value);
StackResult stack_peek_checked(const Stack *st, int *out_value);

#endif // STACK_H
