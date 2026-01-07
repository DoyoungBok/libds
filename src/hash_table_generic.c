#include "hash_table_generic.h"
#include <stdlib.h>
#include <string.h>

/* ---------- Internal types ---------- */

typedef struct HTGNode {
    char *key;              // owned deep copy
    void *value;            // generic pointer
    struct HTGNode *next;
} HTGNode;

struct HashTableG {
    HTGNode **buckets;
    size_t capacity;
    size_t size;
    htg_value_destructor destructor; // may be NULL
};

/* ---------- Internal helpers ---------- */

static size_t htg_default_capacity(void) {
    return 16;
}

static unsigned long hash_djb2(const char *str) {
    unsigned long h = 5381;
    unsigned char c;
    while ((c = (unsigned char)*str++)) {
        h = ((h << 5) + h) + c; // h * 33 + c
    }
    return h;
}

static size_t bucket_index(const HashTableG *ht, const char *key) {
    return (size_t)(hash_djb2(key) % ht->capacity);
}

static char *strdup_portable(const char *s) {
    size_t n = strlen(s);
    char *copy = malloc(n + 1);
    if (!copy) return NULL;
    memcpy(copy, s, n + 1);
    return copy;
}

static HTGNode* node_create(const char *key, void *value) {
    HTGNode *node = malloc(sizeof(HTGNode));
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

static void node_destroy(HashTableG *ht, HTGNode *node) {
    if (!node) return;
    if (ht && ht->destructor && node->value) {
        ht->destructor(node->value);
    }
    free(node->key);
    free(node);
}

static HTGNode* chain_find(HTGNode *head, const char *key) {
    for (HTGNode *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) return cur;
    }
    return NULL;
}

static double load_factor(const HashTableG *ht) {
    if (!ht || ht->capacity == 0) return 0.0;
    return (double)ht->size / (double)ht->capacity;
}

static HTGResult htg_rehash(HashTableG *ht, size_t new_capacity) {
    if (!ht) return HTG_ERR_NULL;
    if (new_capacity < 8) new_capacity = 8;

    HTGNode **new_buckets = calloc(new_capacity, sizeof(HTGNode*));
    if (!new_buckets) return HTG_ERR_ALLOC;

    for (size_t i = 0; i < ht->capacity; i++) {
        HTGNode *cur = ht->buckets[i];
        while (cur) {
            HTGNode *next = cur->next;

            size_t new_idx = (size_t)(hash_djb2(cur->key) % new_capacity);
            cur->next = new_buckets[new_idx];
            new_buckets[new_idx] = cur;

            cur = next;
        }
    }

    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->capacity = new_capacity;
    return HTG_OK;
}

static HTGResult htg_maybe_grow(HashTableG *ht) {
    if (!ht) return HTG_ERR_NULL;
    if (load_factor(ht) <= 0.75) return HTG_OK;
    return htg_rehash(ht, ht->capacity * 2);
}

/* ---------- Public API ---------- */

HashTableG* htg_create(size_t initial_capacity, htg_value_destructor destructor) {
    if (initial_capacity == 0) initial_capacity = htg_default_capacity();
    if (initial_capacity < 8) initial_capacity = 8;

    HashTableG *ht = malloc(sizeof(HashTableG));
    if (!ht) return NULL;

    ht->buckets = calloc(initial_capacity, sizeof(HTGNode*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }

    ht->capacity = initial_capacity;
    ht->size = 0;
    ht->destructor = destructor;
    return ht;
}

void htg_destroy(HashTableG *ht) {
    if (!ht) return;

    for (size_t i = 0; i < ht->capacity; i++) {
        HTGNode *cur = ht->buckets[i];
        while (cur) {
            HTGNode *next = cur->next;
            node_destroy(ht, cur);
            cur = next;
        }
    }

    free(ht->buckets);
    free(ht);
}

size_t htg_size(const HashTableG *ht) {
    return ht ? ht->size : 0;
}

/* ===================== Unsafe API ===================== */

void htg_set(HashTableG *ht, const char *key, void *value) {
    (void)htg_set_try(ht, key, value);
}

bool htg_get(const HashTableG *ht, const char *key, void **out_value) {
    return htg_get_checked(ht, key, out_value) == HTG_OK;
}

bool htg_remove(HashTableG *ht, const char *key) {
    return htg_remove_checked(ht, key) == HTG_OK;
}

/* ===================== Safe API ===================== */

HTGResult htg_set_try(HashTableG *ht, const char *key, void *value) {
    if (!ht || !key) return HTG_ERR_NULL;
    if (key[0] == '\0') return HTG_ERR_INVALID;

    HTGResult gr = htg_maybe_grow(ht);
    if (gr != HTG_OK) return gr;

    size_t idx = bucket_index(ht, key);
    HTGNode *existing = chain_find(ht->buckets[idx], key);
    if (existing) {
        /* Overwrite: destroy old value if we own it */
        if (ht->destructor && existing->value) {
            ht->destructor(existing->value);
        }
        existing->value = value;
        return HTG_OK;
    }

    HTGNode *node = node_create(key, value);
    if (!node) return HTG_ERR_ALLOC;

    node->next = ht->buckets[idx];
    ht->buckets[idx] = node;
    ht->size++;
    return HTG_OK;
}

HTGResult htg_get_checked(const HashTableG *ht, const char *key, void **out_value) {
    if (!ht || !key || !out_value) return HTG_ERR_NULL;
    if (key[0] == '\0') return HTG_ERR_INVALID;

    size_t idx = bucket_index(ht, key);
    HTGNode *n = chain_find(ht->buckets[idx], key);
    if (!n) return HTG_ERR_NOT_FOUND;

    *out_value = n->value;
    return HTG_OK;
}

HTGResult htg_remove_checked(HashTableG *ht, const char *key) {
    if (!ht || !key) return HTG_ERR_NULL;
    if (key[0] == '\0') return HTG_ERR_INVALID;

    size_t idx = bucket_index(ht, key);
    HTGNode *cur = ht->buckets[idx];
    HTGNode *prev = NULL;

    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else ht->buckets[idx] = cur->next;

            node_destroy(ht, cur);
            ht->size--;
            return HTG_OK;
        }
        prev = cur;
        cur = cur->next;
    }

    return HTG_ERR_NOT_FOUND;
}
/* ===================== Iteration API ===================== */

HashTableGIter htg_iter_begin(const HashTableG *ht) {
    HashTableGIter it;
    it.ht = ht;
    it.bucket_index = 0;
    it.node = NULL;
    return it;
}

bool htg_iter_next(HashTableGIter *it, const char **out_key, void **out_value) {
    if (!it || !it->ht || !out_key || !out_value) return false;

    const HashTableG *ht = it->ht;

    /* If we have a current node, try moving within the same chain first */
    if (it->node) {
        HTGNode *cur = (HTGNode*)it->node;
        if (cur->next) {
            cur = cur->next;
            it->node = (void*)cur;
            *out_key = cur->key;
            *out_value = cur->value;
            return true;
        }
        /* Otherwise, move to the next bucket */
        it->node = NULL;
        it->bucket_index++;
    }

    /* Find the next non-empty bucket */
    while (it->bucket_index < ht->capacity) {
        HTGNode *head = ht->buckets[it->bucket_index];
        if (head) {
            it->node = (void*)head;
            *out_key = head->key;
            *out_value = head->value;
            return true;
        }
        it->bucket_index++;
    }

    return false; // finished
}
