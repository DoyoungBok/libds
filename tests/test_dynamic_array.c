#include <assert.h>
#include <stdio.h>
#include "dynamic_array.h"

/*
 * A simple test suite for the DynamicArray module.
 * Uses assert() to validate expected behavior.
 *
 * How to run:
 * 1) make
 * 2) ./test_dynamic_array
 */

static void test_create_and_destroy(void) {
    DynamicArray *da = da_create(2);
    assert(da != NULL);
    assert(da_size(da) == 0);
    da_destroy(da);
}

static void test_append_and_get(void) {
    DynamicArray *da = da_create(2);
    assert(da != NULL);

    da_append(da, 10);
    da_append(da, 20);

    assert(da_size(da) == 2);
    assert(da_get(da, 0) == 10);
    assert(da_get(da, 1) == 20);

    da_destroy(da);
}

static void test_resize_on_append(void) {
    /*
     * This test ensures that the array resizes when capacity is exceeded.
     * We start with capacity 2, then append 3 elements.
     */
    DynamicArray *da = da_create(2);
    assert(da != NULL);

    da_append(da, 1);
    da_append(da, 2);
    da_append(da, 3); // should trigger resize internally

    assert(da_size(da) == 3);
    assert(da_get(da, 0) == 1);
    assert(da_get(da, 1) == 2);
    assert(da_get(da, 2) == 3);

    da_destroy(da);
}

static void test_set(void) {
    DynamicArray *da = da_create(2);
    assert(da != NULL);

    da_append(da, 5);
    da_append(da, 6);

    da_set(da, 1, 99);
    assert(da_get(da, 0) == 5);
    assert(da_get(da, 1) == 99);

    da_destroy(da);
}

static void test_insert(void) {
    /*
     * Insert at various positions:
     * - insert at index 0 (front)
     * - insert in the middle
     * - insert at the end (index == size)
     */
    DynamicArray *da = da_create(2);
    assert(da != NULL);

    da_append(da, 10);
    da_append(da, 30);

    da_insert(da, 0, 5);   // [5, 10, 30]
    assert(da_size(da) == 3);
    assert(da_get(da, 0) == 5);
    assert(da_get(da, 1) == 10);
    assert(da_get(da, 2) == 30);

    da_insert(da, 2, 20);  // [5, 10, 20, 30]
    assert(da_size(da) == 4);
    assert(da_get(da, 0) == 5);
    assert(da_get(da, 1) == 10);
    assert(da_get(da, 2) == 20);
    assert(da_get(da, 3) == 30);

    da_insert(da, da_size(da), 40); // append via insert: [5, 10, 20, 30, 40]
    assert(da_size(da) == 5);
    assert(da_get(da, 4) == 40);

    da_destroy(da);
}

static void test_remove(void) {
    /*
     * Remove at various positions:
     * - remove from the middle
     * - remove from the front
     * - remove from the end
     */
    DynamicArray *da = da_create(2);
    assert(da != NULL);

    da_append(da, 10);
    da_append(da, 20);
    da_append(da, 30);
    da_append(da, 40); // [10, 20, 30, 40]

    da_remove(da, 1); // remove 20 -> [10, 30, 40]
    assert(da_size(da) == 3);
    assert(da_get(da, 0) == 10);
    assert(da_get(da, 1) == 30);
    assert(da_get(da, 2) == 40);

    da_remove(da, 0); // remove 10 -> [30, 40]
    assert(da_size(da) == 2);
    assert(da_get(da, 0) == 30);
    assert(da_get(da, 1) == 40);

    da_remove(da, 1); // remove 40 -> [30]
    assert(da_size(da) == 1);
    assert(da_get(da, 0) == 30);

    da_destroy(da);
}

static void test_find(void) {
    DynamicArray *da = da_create(2);
    assert(da != NULL);

    da_append(da, 7);
    da_append(da, 8);
    da_append(da, 9);

    assert(da_find(da, 7) == 0);
    assert(da_find(da, 8) == 1);
    assert(da_find(da, 9) == 2);
    assert(da_find(da, 100) == -1);

    da_destroy(da);
}

static void test_checked_api_bounds(void) {
    DynamicArray *da = da_create(1);
    assert(da != NULL);

    int out = 0;

    /* get on empty should fail with out-of-bounds */
    assert(da_get_checked(da, 0, &out) == DA_ERR_OOB);

    /* append_try should succeed */
    assert(da_append_try(da, 123) == DA_OK);
    assert(da_get_checked(da, 0, &out) == DA_OK);
    assert(out == 123);

    /* set_checked out-of-bounds should fail */
    assert(da_set_checked(da, 1, 999) == DA_ERR_OOB);

    /* insert_try at index > size should fail */
    assert(da_insert_try(da, 2, 5) == DA_ERR_OOB);

    /* remove_checked out-of-bounds should fail */
    assert(da_remove_checked(da, 1) == DA_ERR_OOB);

    /* remove_checked valid index should succeed */
    assert(da_remove_checked(da, 0) == DA_OK);
    assert(da_remove_checked(da, 0) == DA_ERR_EMPTY);

    da_destroy(da);
}


int main(void) {
    test_create_and_destroy();
    test_append_and_get();
    test_resize_on_append();
    test_set();
    test_insert();
    test_remove();
    test_find();
    test_checked_api_bounds();

    printf("All DynamicArray tests passed!\n");
    return 0;
}
