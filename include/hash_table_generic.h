#ifndef HASH_TABLE_GENERIC_H
#define HASH_TABLE_GENERIC_H

#include <stddef.h>
#include <stdbool.h>

/*
 * Generic Hash Table (string -> void*)
 * -----------------------------------
 * - Keys are strings, deep-copied and owned by the table.
 * - Values are void* pointers.
 * - The table can optionally take ownership of values via a destructor callback.
 *
 * This design is common in C libraries: the user decides whether the table owns values.
 */

typedef struct HashTableG HashTableG;

/*
 * User-provided destructor for values.
 * If provided, it will be called on value pointers when:
 * - a key is overwritten with a new value
 * - a key is removed
 * - the table is destroyed
 */
typedef void (*htg_value_destructor)(void *value);

typedef enum {
    HTG_OK = 0,
    HTG_ERR_NULL,
    HTG_ERR_ALLOC,
    HTG_ERR_NOT_FOUND,
    HTG_ERR_INVALID
} HTGResult;

/* Constructor / Destructor */
HashTableG* htg_create(size_t initial_capacity, htg_value_destructor destructor);
void htg_destroy(HashTableG *ht);

/* Basic info */
size_t htg_size(const HashTableG *ht);

/* Unsafe operations */
void htg_set(HashTableG *ht, const char *key, void *value);
bool htg_get(const HashTableG *ht, const char *key, void **out_value);
bool htg_remove(HashTableG *ht, const char *key);

/* Safe operations */
HTGResult htg_set_try(HashTableG *ht, const char *key, void *value);
HTGResult htg_get_checked(const HashTableG *ht, const char *key, void **out_value);
HTGResult htg_remove_checked(HashTableG *ht, const char *key);


/* ============================================================
 *                        Iteration
 * ============================================================
 * Allows iterating over all key-value pairs without exposing internal structures.
 */

typedef struct {
    const HashTableG *ht;  // table being iterated
    size_t bucket_index;   // current bucket position
    void *node;            // internal pointer to current node (opaque to user)
} HashTableGIter;

/*
 * Initializes an iterator. After init, call htg_iter_next() repeatedly.
 */
HashTableGIter htg_iter_begin(const HashTableG *ht);

/*
 * Advances the iterator.
 * Returns true if an element is produced, false when iteration is finished.
 *
 * out_key and out_value are set to point to the current entry.
 * out_key points to the internal key string (do not free/modify it).
 * out_value is the stored value pointer.
 */
bool htg_iter_next(HashTableGIter *it, const char **out_key, void **out_value);

#endif // HASH_TABLE_GENERIC_H
