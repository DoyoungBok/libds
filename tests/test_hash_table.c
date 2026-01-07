#include <assert.h>
#include <stdio.h>
#include "hash_table.h"

static void test_set_get_update(void) {
    HashTable *ht = ht_create(4);
    assert(ht != NULL);

    int out = 0;

    assert(ht_set_try(ht, "apple", 10) == HT_OK);
    assert(ht_set_try(ht, "banana", 20) == HT_OK);
    assert(ht_size(ht) == 2);

    assert(ht_get_checked(ht, "apple", &out) == HT_OK);
    assert(out == 10);

    /* update */
    assert(ht_set_try(ht, "apple", 99) == HT_OK);
    assert(ht_get_checked(ht, "apple", &out) == HT_OK);
    assert(out == 99);

    ht_destroy(ht);
}

static void test_remove_and_not_found(void) {
    HashTable *ht = ht_create(8);
    assert(ht != NULL);

    int out = 0;

    assert(ht_set_try(ht, "x", 1) == HT_OK);
    assert(ht_set_try(ht, "y", 2) == HT_OK);
    assert(ht_size(ht) == 2);

    assert(ht_remove_checked(ht, "x") == HT_OK);
    assert(ht_size(ht) == 1);

    assert(ht_get_checked(ht, "x", &out) == HT_ERR_NOT_FOUND);
    assert(ht_remove_checked(ht, "x") == HT_ERR_NOT_FOUND);

    ht_destroy(ht);
}

static void test_rehash_grow(void) {
    /*
     * Insert enough keys to trigger rehash.
     * The table should still return correct values afterward.
     */
    HashTable *ht = ht_create(8);
    assert(ht != NULL);

    char key[64];
    for (int i = 0; i < 200; i++) {
        snprintf(key, sizeof(key), "k%d", i);
        assert(ht_set_try(ht, key, i * 3) == HT_OK);
    }
    assert(ht_size(ht) == 200);

    for (int i = 0; i < 200; i++) {
        int out = 0;
        snprintf(key, sizeof(key), "k%d", i);
        assert(ht_get_checked(ht, key, &out) == HT_OK);
        assert(out == i * 3);
    }

    ht_destroy(ht);
}

static void test_invalid_inputs(void) {
    HashTable *ht = ht_create(8);
    assert(ht != NULL);

    int out = 0;
    assert(ht_get_checked(NULL, "a", &out) == HT_ERR_NULL);
    assert(ht_get_checked(ht, NULL, &out) == HT_ERR_NULL);
    assert(ht_get_checked(ht, "", &out) == HT_ERR_INVALID);
    assert(ht_set_try(ht, "", 1) == HT_ERR_INVALID);

    ht_destroy(ht);
}

int main(void) {
    test_set_get_update();
    test_remove_and_not_found();
    test_rehash_grow();
    test_invalid_inputs();

    printf("All HashTable tests passed!\n");
    return 0;
}
