#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table_generic.h"

/*
 * We track how many times the destructor is called to ensure ownership is correct.
 */
static int g_destructor_calls = 0;

static void int_destructor(void *p) {
    g_destructor_calls++;
    free(p);
}

static int* heap_int(int x) {
    int *p = malloc(sizeof(int));
    assert(p != NULL);
    *p = x;
    return p;
}

static void test_set_get_and_overwrite(void) {
    g_destructor_calls = 0;

    HashTableG *ht = htg_create(8, int_destructor);
    assert(ht != NULL);

    void *out = NULL;

    assert(htg_set_try(ht, "a", heap_int(10)) == HTG_OK);
    assert(htg_get_checked(ht, "a", &out) == HTG_OK);
    assert(*(int*)out == 10);

    /* Overwrite: old value should be destroyed */
    assert(htg_set_try(ht, "a", heap_int(99)) == HTG_OK);
    assert(htg_get_checked(ht, "a", &out) == HTG_OK);
    assert(*(int*)out == 99);
    assert(g_destructor_calls == 1);

    htg_destroy(ht);

    /* Destroy should destroy the remaining value */
    assert(g_destructor_calls == 2);
}

static void test_remove_calls_destructor(void) {
    g_destructor_calls = 0;

    HashTableG *ht = htg_create(8, int_destructor);
    assert(ht != NULL);

    assert(htg_set_try(ht, "x", heap_int(1)) == HTG_OK);
    assert(htg_set_try(ht, "y", heap_int(2)) == HTG_OK);
    assert(htg_size(ht) == 2);

    assert(htg_remove_checked(ht, "x") == HTG_OK);
    assert(htg_size(ht) == 1);
    assert(g_destructor_calls == 1);

    assert(htg_remove_checked(ht, "x") == HTG_ERR_NOT_FOUND);

    htg_destroy(ht);
    assert(g_destructor_calls == 2);
}

static void test_no_destructor_mode(void) {
    /*
     * If destructor is NULL, the table does NOT free values.
     * The caller remains responsible for freeing them.
     */
    HashTableG *ht = htg_create(8, NULL);
    assert(ht != NULL);

    int *p = heap_int(123);
    assert(htg_set_try(ht, "k", p) == HTG_OK);

    void *out = NULL;
    assert(htg_get_checked(ht, "k", &out) == HTG_OK);
    assert(out == p);

    htg_destroy(ht);

    /* Caller must free p */
    free(p);
}

static void test_iteration_counts_all_entries(void) {
    HashTableG *ht = htg_create(8, int_destructor);
    assert(ht != NULL);

    /* Insert a bunch of entries */
    for (int i = 0; i < 50; i++) {
        char key[64];
        snprintf(key, sizeof(key), "key%d", i);
        assert(htg_set_try(ht, key, heap_int(i)) == HTG_OK);
    }
    assert(htg_size(ht) == 50);

    /* Iterate and count */
    size_t count = 0;
    HashTableGIter it = htg_iter_begin(ht);

    const char *k = NULL;
    void *v = NULL;

    while (htg_iter_next(&it, &k, &v)) {
        assert(k != NULL);
        assert(v != NULL);
        count++;
    }

    assert(count == 50);

    htg_destroy(ht);
}


int main(void) {
    test_set_get_and_overwrite();
    test_remove_calls_destructor();
    test_no_destructor_mode();
    test_iteration_counts_all_entries();

    printf("All Generic HashTable tests passed!\n");
    return 0;
}
