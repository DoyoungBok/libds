#include <assert.h>
#include <stdio.h>
#include "linked_list.h"

static void test_basic_operations(void) {
    LinkedList *list = ll_create();
    assert(list != NULL);
    assert(ll_size(list) == 0);

    ll_push_back(list, 10);
    ll_push_front(list, 5);
    ll_push_back(list, 20);

    assert(ll_size(list) == 3);
    assert(ll_get(list, 0) == 5);
    assert(ll_get(list, 1) == 10);
    assert(ll_get(list, 2) == 20);

    ll_destroy(list);
}

static void test_safe_api(void) {
    LinkedList *list = ll_create();
    assert(list != NULL);

    int out = 0;
    assert(ll_get_checked(list, 0, &out) == LL_ERR_OOB);

    assert(ll_push_back_try(list, 42) == LL_OK);
    assert(ll_get_checked(list, 0, &out) == LL_OK);
    assert(out == 42);

    assert(ll_remove_checked(list, 0) == LL_OK);
    assert(ll_remove_checked(list, 0) == LL_ERR_EMPTY);

    ll_destroy(list);
}

int main(void) {
    test_basic_operations();
    test_safe_api();

    printf("All LinkedList tests passed!\n");
    return 0;
}
