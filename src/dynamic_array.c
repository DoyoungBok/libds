#include "dynamic_array.h"
#include <stdlib.h>

/*
 * Internal helper: attempts to resize the array to a new capacity.
 * Returns DA_OK on success, or DA_ERR_ALLOC on failure.
 */
static DAResult da_resize_to(DynamicArray *da, size_t new_capacity) {
    if (!da) return DA_ERR_NULL;
    if (new_capacity < da->size) new_capacity = da->size;

    int *new_data = realloc(da->data, sizeof(int) * new_capacity);
    if (!new_data) return DA_ERR_ALLOC;

    da->data = new_data;
    da->capacity = new_capacity;
    return DA_OK;
}

/*
 * Internal helper: ensures there is room for one more element.
 */
static DAResult da_ensure_capacity_for_one_more(DynamicArray *da) {
    if (!da) return DA_ERR_NULL;
    if (da->size < da->capacity) return DA_OK;

    size_t new_capacity = (da->capacity == 0) ? 1 : da->capacity * 2;
    return da_resize_to(da, new_capacity);
}

DynamicArray* da_create(size_t capacity) {
    if (capacity == 0) capacity = 1;

    DynamicArray *da = malloc(sizeof(DynamicArray));
    if (!da) return NULL;

    da->data = malloc(sizeof(int) * capacity);
    if (!da->data) {
        free(da);
        return NULL;
    }

    da->size = 0;
    da->capacity = capacity;
    return da;
}

void da_destroy(DynamicArray *da) {
    if (!da) return;
    free(da->data);
    free(da);
}

/* ===================== Unsafe API ===================== */

void da_append(DynamicArray *da, int value) {
    (void)da_append_try(da, value);
}

int da_get(const DynamicArray *da, size_t index) {
    return da->data[index];
}

void da_set(DynamicArray *da, size_t index, int value) {
    da->data[index] = value;
}

size_t da_size(const DynamicArray *da) {
    return da ? da->size : 0;
}

void da_insert(DynamicArray *da, size_t index, int value) {
    (void)da_insert_try(da, index, value);
}

void da_remove(DynamicArray *da, size_t index) {
    (void)da_remove_checked(da, index);
}

int da_find(const DynamicArray *da, int value) {
    if (!da) return -1;
    for (size_t i = 0; i < da->size; i++) {
        if (da->data[i] == value) return (int)i;
    }
    return -1;
}

/* ===================== Safe API ===================== */

DAResult da_append_try(DynamicArray *da, int value) {
    if (!da) return DA_ERR_NULL;

    DAResult r = da_ensure_capacity_for_one_more(da);
    if (r != DA_OK) return r;

    da->data[da->size++] = value;
    return DA_OK;
}

DAResult da_get_checked(const DynamicArray *da, size_t index, int *out_value) {
    if (!da || !out_value) return DA_ERR_NULL;
    if (index >= da->size) return DA_ERR_OOB;

    *out_value = da->data[index];
    return DA_OK;
}

DAResult da_set_checked(DynamicArray *da, size_t index, int value) {
    if (!da) return DA_ERR_NULL;
    if (index >= da->size) return DA_ERR_OOB;

    da->data[index] = value;
    return DA_OK;
}

DAResult da_insert_try(DynamicArray *da, size_t index, int value) {
    if (!da) return DA_ERR_NULL;
    if (index > da->size) return DA_ERR_OOB; // allow index == size (append)

    DAResult r = da_ensure_capacity_for_one_more(da);
    if (r != DA_OK) return r;

    for (size_t i = da->size; i > index; i--) {
        da->data[i] = da->data[i - 1];
    }
    da->data[index] = value;
    da->size++;
    return DA_OK;
}

DAResult da_remove_checked(DynamicArray *da, size_t index) {
    if (!da) return DA_ERR_NULL;
    if (da->size == 0) return DA_ERR_EMPTY;
    if (index >= da->size) return DA_ERR_OOB;

    for (size_t i = index; i + 1 < da->size; i++) {
        da->data[i] = da->data[i + 1];
    }
    da->size--;
    return DA_OK;
}
