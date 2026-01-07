#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h> // size_t
#include <stdbool.h>

/*
 * HashTable
 * ---------
 * A hash table mapping string keys to int values.
 * Collision resolution: separate chaining (linked lists per bucket).
 *
 * The table owns copies of keys (deep copy), so caller can free their key strings.
 */

typedef struct HashTable HashTable;

/*
 * HTResult
 * --------
 * Return codes for safe hash table operations.
 */
typedef enum {
    HT_OK = 0,
    HT_ERR_NULL,
    HT_ERR_ALLOC,
    HT_ERR_NOT_FOUND,
    HT_ERR_INVALID
} HTResult;


/* ============================================================
 *                   Constructor / Destructor
 * ============================================================ */

/*
 * Creates a new hash table with at least `initial_capacity` buckets.
 * If initial_capacity is 0, it will be upgraded to a reasonable default.
 * Returns NULL on allocation failure.
 */
HashTable* ht_create(size_t initial_capacity);

/*
 * Frees the entire hash table including all nodes and key copies.
 * Safe to call with NULL.
 */
void ht_destroy(HashTable *ht);


/* ============================================================
 *                     Unsafe Operations
 * ============================================================
 * These functions may skip error handling for convenience.
 * Prefer *_try versions if you want explicit errors.
 */

/* Inserts or updates (key -> value). */
void ht_set(HashTable *ht, const char *key, int value);

/* Returns true if key exists, and writes value to out_value. */
bool ht_get(const HashTable *ht, const char *key, int *out_value);

/* Removes a key if present; returns true if removed. */
bool ht_remove(HashTable *ht, const char *key);

/* Returns number of stored key-value pairs. */
size_t ht_size(const HashTable *ht);


/* ============================================================
 *                     Safe Operations
 * ============================================================ */

HTResult ht_set_try(HashTable *ht, const char *key, int value);
HTResult ht_get_checked(const HashTable *ht, const char *key, int *out_value);
HTResult ht_remove_checked(HashTable *ht, const char *key);

#endif // HASH_TABLE_H
