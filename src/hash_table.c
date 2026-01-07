#include "hash_table.h"
#include <stdlib.h>
#include <string.h>

/* ---------- Internal types ---------- */

typedef struct HTNode {
    char *key;              // owned key copy
    int value;
    struct HTNode *next;
} HTNode;

struct HashTable {
    HTNode **buckets;
    size_t capacity;        // number of buckets
    size_t size;            // number of entries
};

/* ---------- Internal helpers ---------- */

static size_t ht_default_capacity(void) {
    return 16;
}

/*
 * djb2 string hash (simple and common).
 */
static unsigned long hash_djb2(const char *str) {
    unsigned long h = 5381;
    unsigned char c;
    while ((c = (unsigned char)*str++)) {
        h = ((h << 5) + h) + c; // h * 33 + c
    }
    return h;
}

static size_t bucket_index(const HashTable *ht, const char *key) {
    return (size_t)(hash_djb2(key) % ht->capacity);
}

/*
 * strdup is POSIX; implement a portable version.
 */
static char *strdup_portable(const char *s) {
    size_t n = strlen(s);
    char *copy = malloc(n + 1);
    if (!copy) return NULL;
    memcpy(copy, s, n + 1);
    return copy;
}

static HTNode* node_create(const char *key, int value) {
    HTNode *node = malloc(sizeof(HTNode));
    if (!node) return NULL;

    node->key = strdup_portable(key);
    if (!node->key) {
        free(node);
        return NULL;
    }

    node->value = value;
    node->next = NULL;
    return node;
}

static void node_destroy(HTNode *node) {
    if (!node) return;
    free(node->key);
    free(node);
}

/*
 * Finds the node for key in a bucket chain.
 * Returns pointer to node, or NULL if not found.
 */
static HTNode* chain_find(HTNode *head, const char *key) {
    for (HTNode *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) return cur;
    }
    return NULL;
}

/*
 * Load factor threshold for resizing.
 */
static double load_factor(const HashTable *ht) {
    if (!ht || ht->capacity == 0) return 0.0;
    return (double)ht->size / (double)ht->capacity;
}

/*
 * Rehash: allocate new buckets and move nodes.
 * Returns HT_OK or HT_ERR_ALLOC.
 */
static HTResult ht_rehash(HashTable *ht, size_t new_capacity) {
    if (!ht) return HT_ERR_NULL;
    if (new_capacity < 8) new_capacity = 8;

    HTNode **new_buckets = calloc(new_capacity, sizeof(HTNode*));
    if (!new_buckets) return HT_ERR_ALLOC;

    /* Move nodes from old buckets to new buckets */
    for (size_t i = 0; i < ht->capacity; i++) {
        HTNode *cur = ht->buckets[i];
        while (cur) {
            HTNode *next = cur->next;

            size_t new_idx = (size_t)(hash_djb2(cur->key) % new_capacity);
            cur->next = new_buckets[new_idx];
            new_buckets[new_idx] = cur;

            cur = next;
        }
    }

    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->capacity = new_capacity;
    return HT_OK;
}

static HTResult ht_maybe_grow(HashTable *ht) {
    if (!ht) return HT_ERR_NULL;
    if (load_factor(ht) <= 0.75) return HT_OK;
    return ht_rehash(ht, ht->capacity * 2);
}

/* ---------- Public API ---------- */

HashTable* ht_create(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = ht_default_capacity();
    if (initial_capacity < 8) initial_capacity = 8;

    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;

    ht->buckets = calloc(initial_capacity, sizeof(HTNode*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }

    ht->capacity = initial_capacity;
    ht->size = 0;
    return ht;
}

void ht_destroy(HashTable *ht) {
    if (!ht) return;

    for (size_t i = 0; i < ht->capacity; i++) {
        HTNode *cur = ht->buckets[i];
        while (cur) {
            HTNode *next = cur->next;
            node_destroy(cur);
            cur = next;
        }
    }
    free(ht->buckets);
    free(ht);
}

size_t ht_size(const HashTable *ht) {
    return ht ? ht->size : 0;
}

/* ===================== Unsafe API ===================== */

void ht_set(HashTable *ht, const char *key, int value) {
    (void)ht_set_try(ht, key, value);
}

bool ht_get(const HashTable *ht, const char *key, int *out_value) {
    return ht_get_checked(ht, key, out_value) == HT_OK;
}

bool ht_remove(HashTable *ht, const char *key) {
    return ht_remove_checked(ht, key) == HT_OK;
}

/* ===================== Safe API ===================== */

HTResult ht_set_try(HashTable *ht, const char *key, int value) {
    if (!ht || !key) return HT_ERR_NULL;
    if (key[0] == '\0') return HT_ERR_INVALID;

    HTResult gr = ht_maybe_grow(ht);
    if (gr != HT_OK) return gr;

    size_t idx = bucket_index(ht, key);
    HTNode *existing = chain_find(ht->buckets[idx], key);
    if (existing) {
        existing->value = value;
        return HT_OK;
    }

    HTNode *node = node_create(key, value);
    if (!node) return HT_ERR_ALLOC;

    node->next = ht->buckets[idx];
    ht->buckets[idx] = node;
    ht->size++;
    return HT_OK;
}

HTResult ht_get_checked(const HashTable *ht, const char *key, int *out_value) {
    if (!ht || !key || !out_value) return HT_ERR_NULL;
    if (key[0] == '\0') return HT_ERR_INVALID;

    size_t idx = bucket_index(ht, key);
    HTNode *n = chain_find(ht->buckets[idx], key);
    if (!n) return HT_ERR_NOT_FOUND;

    *out_value = n->value;
    return HT_OK;
}

HTResult ht_remove_checked(HashTable *ht, const char *key) {
    if (!ht || !key) return HT_ERR_NULL;
    if (key[0] == '\0') return HT_ERR_INVALID;

    size_t idx = bucket_index(ht, key);
    HTNode *cur = ht->buckets[idx];
    HTNode *prev = NULL;

    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else ht->buckets[idx] = cur->next;

            node_destroy(cur);
            ht->size--;
            return HT_OK;
        }
        prev = cur;
        cur = cur->next;
    }

    return HT_ERR_NOT_FOUND;
}
