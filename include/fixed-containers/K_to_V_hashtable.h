/*
    `K_to_V_hashtable` is a hashmap implementation based on a fixed size array.

    At least one slot in the hashmap is expected to be reserved - so searching
    for missing keys works. Load factor should not exactly be equal to 1.
    Use `init_with_capacity_rounded_up` if unsure.

    Reference:
    https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/
    https://thenumb.at/hashtables/#robin-hood-linear-probing
*/

#ifndef __K_TO_V_HASHTABLE__H
#define __K_TO_V_HASHTABLE__H

#include "is_pow2.h"    // is_pow2
#include "murmurhash.h" // murmurhash
#include "nextpow2.h"   // nextpow2
#include <assert.h>     // assert
#include <stdbool.h>    // bool, true, false
#include <stddef.h>     // offsetof
#include <stdint.h>     // SIZE_MAX, uint64_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // size_t, NULL, malloc, free
#include <string.h>     // memcpy

#endif

#ifndef KEY_TYPE
#error "Must declare KEY_TYPE. Defaulting to int."
#define KEY_TYPE int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#ifndef KEY_IS_EQUAL
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#endif

#ifndef HASH_FUNCTION
#define HASH_FUNCTION(key) murmurhash((const char*)&(key), sizeof(KEY_TYPE), 0)
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define K_to_V_hashtable JOIN(KEY_TYPE, JOIN(JOIN(to, VALUE_TYPE), hashtable))
#define K_to_V_hashtable_type JOIN(K_to_V_hashtable, type)
#define K_to_V_hashtable_slot JOIN(K_to_V_hashtable, slot)
#define K_to_V_hashtable_slot_type JOIN(K_to_V_hashtable_slot, type)

#define EMPTY_SLOT_OFFSET SIZE_MAX

typedef struct {
    size_t offset;
    KEY_TYPE key;
    VALUE_TYPE value;
} K_to_V_hashtable_slot_type;

typedef struct {
    size_t count;
    size_t index_mask;
    K_to_V_hashtable_slot_type arr[];
} K_to_V_hashtable_type;

static inline bool JOIN(K_to_V_hashtable, init_internal)(K_to_V_hashtable_type** hashtable_pp, size_t pow2_capacity) {
    assert(hashtable_pp != NULL);
    assert(is_pow2(pow2_capacity) && "initial capacity is a power of 2");
    assert(pow2_capacity - 1 != 0 && "subtracting initial capacity by one does not yield zero");

    if (pow2_capacity > (SIZE_MAX - offsetof(K_to_V_hashtable_type, arr)) / sizeof(K_to_V_hashtable_slot_type)) {
        return false;
    }
    *hashtable_pp = malloc(offsetof(K_to_V_hashtable_type, arr) + sizeof(K_to_V_hashtable_slot_type) * pow2_capacity);
    if ((*hashtable_pp) == NULL) {
        return false;
    }
    (*hashtable_pp)->count = 0;
    (*hashtable_pp)->index_mask = pow2_capacity - 1;

    K_to_V_hashtable_type* hashtable_p = *hashtable_pp;
    for (size_t i = 0; i < pow2_capacity; i++) {
        hashtable_p->arr[i].offset = EMPTY_SLOT_OFFSET;
    }

    return true;
}

static inline bool JOIN(K_to_V_hashtable, init)(K_to_V_hashtable_type** queue_pp, size_t capacity) {
    assert(queue_pp != NULL);

    if (capacity == 0) {
        return false;
    } else if (capacity == 1) {
        capacity = 2;
    } else if (is_pow2(capacity)) {
        capacity += 1;
    }
    size_t rounded_capacity = nextpow2(capacity);
    if (rounded_capacity < capacity) {
        return false;
    }

    return JOIN(K_to_V_hashtable, init_internal)(queue_pp, rounded_capacity);
}

static inline bool JOIN(K_to_V_hashtable, deinit)(K_to_V_hashtable_type** queue_pp) {
    assert(queue_pp != NULL);

    if (*queue_pp == NULL) {
        return false;
    }
    free(*queue_pp);
    *queue_pp = NULL;

    return true;
}

static inline bool JOIN(K_to_V_hashtable, copy)(K_to_V_hashtable_type** hashtable_dest_pp, K_to_V_hashtable_type* hashtable_src_p) {
    assert(hashtable_src_p != NULL);
    assert(hashtable_dest_pp != NULL);

    if (!JOIN(K_to_V_hashtable, init_internal)(hashtable_dest_pp, hashtable_src_p->index_mask + 1)) {
        return false;
    }
    memcpy((*hashtable_dest_pp)->arr, hashtable_src_p->arr, sizeof(K_to_V_hashtable_slot_type) * (hashtable_src_p->index_mask + 1));
    (*hashtable_dest_pp)->count = hashtable_src_p->count;

    return true;
}

static inline size_t JOIN(K_to_V_hashtable, get_count)(const K_to_V_hashtable_type* hashtable_p) {
    assert(hashtable_p != NULL);

    return hashtable_p->count;
}

static inline size_t JOIN(K_to_V_hashtable, get_capacity)(const K_to_V_hashtable_type* hashtable_p) {
    assert(hashtable_p != NULL);

    return hashtable_p->index_mask + 1;
}

static inline bool JOIN(K_to_V_hashtable, is_empty)(const K_to_V_hashtable_type* hashtable_p) {
    assert(hashtable_p != NULL);

    return hashtable_p->count == 0;
}

static inline bool JOIN(K_to_V_hashtable, is_full)(const K_to_V_hashtable_type* hashtable_p) {
    assert(hashtable_p != NULL);

    return hashtable_p->count == hashtable_p->index_mask + 1;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

static inline const bool JOIN(K_to_V_hashtable, exists)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false && "loops forever for missing keys. consider using a larger hashtable.");
#undef K_to_V_hashtable_is_full

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    size_t dist = 0;

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET && dist <= hashtable_p->arr[index].offset) {
        if (KEY_IS_EQUAL(hashtable_p->arr[index].key, key)) {
            return true;
        }
        index = (index + 1) & hashtable_p->index_mask;
        dist++;
    }
    return false;
}

static inline const VALUE_TYPE JOIN(K_to_V_hashtable, get)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key,
                                                           const VALUE_TYPE default_return_value) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false && "loops forever for missing keys. consider using a larger hashtable.");
#undef K_to_V_hashtable_is_full

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    size_t dist = 0;

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET && dist <= hashtable_p->arr[index].offset) {
        if (KEY_IS_EQUAL(hashtable_p->arr[index].key, key)) {
            return hashtable_p->arr[index].value;
        }
        index = (index + 1) & hashtable_p->index_mask;
        dist++;
    }
    return default_return_value;
}

#pragma GCC diagnostic pop

static inline void JOIN(K_to_V_hashtable, set)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key, const VALUE_TYPE value) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false);
#undef K_to_V_hashtable_is_full

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    K_to_V_hashtable_slot_type slot = {.offset = 0, .key = key, .value = value};

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET) {
        // swap if current_dist is larger. will ensure the maximum
        // offset is minimized.
        if (slot.offset > hashtable_p->arr[index].offset) {
            // swap entries:
            K_to_V_hashtable_slot_type temp = hashtable_p->arr[index];
            hashtable_p->arr[index] = slot;
            slot = temp;
        }
        index = (index + 1) & hashtable_p->index_mask;
        slot.offset++;
    }
    hashtable_p->arr[index] = slot;
    hashtable_p->count++;
}

static inline bool JOIN(K_to_V_hashtable, del)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_empty JOIN(K_to_V_hashtable, is_empty)
    assert(K_to_V_hashtable_is_empty(hashtable_p) == false);
#undef K_to_V_hashtable_is_empty

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    size_t dist = 0;

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET && dist <= hashtable_p->arr[index].offset) {
        if (KEY_IS_EQUAL(hashtable_p->arr[index].key, key)) {
            size_t next_index = (index + 1) & hashtable_p->index_mask;
            while (hashtable_p->arr[next_index].offset != EMPTY_SLOT_OFFSET && hashtable_p->arr[next_index].offset > 0) {
                hashtable_p->arr[index] = hashtable_p->arr[next_index];
                hashtable_p->arr[index].offset--;
                index = next_index;
                next_index = (index + 1) & hashtable_p->index_mask;
            }
            hashtable_p->arr[index].offset = EMPTY_SLOT_OFFSET;
            hashtable_p->count--;
            return true;
        }
        index = (index + 1) & hashtable_p->index_mask;
        dist++;
    }
    return false;
}

#undef EMPTY_SLOT_OFFSET

#undef K_to_V_hashtable
#undef K_to_V_hashtable_type
#undef K_to_V_hashtable_slot
#undef K_to_V_hashtable_slot_type

#undef HASH_FUNCTION
#undef KEY_IS_EQUAL
#undef KEY_TYPE
#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN
