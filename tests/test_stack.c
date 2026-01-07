#include <assert.h>
#include <stdio.h>
#include "stack.h"

static void test_stack_basic(void) {
    Stack *st = stack_create(2);
    assert(st != NULL);
    assert(stack_size(st) == 0);

    stack_push(st, 10);
    stack_push(st, 20);
    stack_push(st, 30);

    assert(stack_size(st) == 3);
    assert(stack_peek(st) == 30);
    assert(stack_pop(st) == 30);
    assert(stack_pop(st) == 20);
    assert(stack_pop(st) == 10);

    stack_destroy(st);
}

static void test_stack_safe_api(void) {
    Stack *st = stack_create(1);
    assert(st != NULL);

    int out = 0;
    assert(stack_pop_checked(st, &out) == STACK_ERR_EMPTY);
    assert(stack_peek_checked(st, &out) == STACK_ERR_EMPTY);

    assert(stack_push_try(st, 7) == STACK_OK);
    assert(stack_peek_checked(st, &out) == STACK_OK);
    assert(out == 7);

    assert(stack_pop_checked(st, &out) == STACK_OK);
    assert(out == 7);
    assert(stack_pop_checked(st, &out) == STACK_ERR_EMPTY);

    stack_destroy(st);
}

int main(void) {
    test_stack_basic();
    test_stack_safe_api();
    printf("All Stack tests passed!\n");
    return 0;
}
