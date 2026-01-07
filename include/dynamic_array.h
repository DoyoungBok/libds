#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h> // for size_t

/*
 * DynamicArray
 * ------------
 * A simple resizable array structure similar to std::vector in C++.
 * Stores integers and manages its own memory using malloc/realloc.
 */
typedef struct {
    int *data;        // Pointer to the underlying storage
    size_t size;      // Number of elements currently stored
    size_t capacity;  // Allocated space (in number of elements)
} DynamicArray;

/*
 * DAResult
 * --------
 * Return codes for "safe" API functions.
 */
typedef enum {
    DA_OK = 0,
    DA_ERR_NULL,        // Null pointer passed in
    DA_ERR_OOB,         // Out-of-bounds index
    DA_ERR_ALLOC,       // Allocation/reallocation failed
    DA_ERR_EMPTY        // Operation not valid on empty array
} DAResult;


/* ============================================================
 *                   Constructor / Destructor
 * ============================================================ */

/*
 * Creates a new dynamic array with a given initial capacity.
 * If capacity is 0, it will be upgraded to 1.
 * Returns NULL if allocation fails.
 */
DynamicArray* da_create(size_t capacity);

/*
 * Frees all memory associated with the dynamic array.
 * Safe to call with NULL.
 */
void da_destroy(DynamicArray *da);


/* ============================================================
 *                     Unsafe Basic Operations
 * ============================================================
 * These functions do NOT perform bounds checks.
 * If you want safety, use the *_checked / *_try versions below.
 */

void da_append(DynamicArray *da, int value);
int da_get(const DynamicArray *da, size_t index);
void da_set(DynamicArray *da, size_t index, int value);
size_t da_size(const DynamicArray *da);

void da_insert(DynamicArray *da, size_t index, int value);
void da_remove(DynamicArray *da, size_t index);
int da_find(const DynamicArray *da, int value);


/* ============================================================
 *                     Safe Operations
 * ============================================================
 * These functions validate inputs and return DAResult.
 */

/* Appends a value; returns error codes instead of exiting. */
DAResult da_append_try(DynamicArray *da, int value);

/* Reads value at index with bounds checking. */
DAResult da_get_checked(const DynamicArray *da, size_t index, int *out_value);

/* Writes value at index with bounds checking. */
DAResult da_set_checked(DynamicArray *da, size_t index, int value);

/* Inserts at index in [0..size]; bounds checked. */
DAResult da_insert_try(DynamicArray *da, size_t index, int value);

/* Removes at index in [0..size-1]; bounds checked. */
DAResult da_remove_checked(DynamicArray *da, size_t index);

#endif // DYNAMIC_ARRAY_H
