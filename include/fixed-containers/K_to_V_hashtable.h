/*
    `K_to_V_hashtable` is a hashtable implementation based on a fixed size array
    using robin hood hashing (open addressing).

    Including this header file generates a struct and functions for a given queue type.

    The following macros gets defined once:
    - EMPTY_SLOT_OFFSET
    - K_to_V_hashtable_for_each

    The following structs are generated for a given key type K and value type V:
    - K_to_V_hashtable_type
    - K_to_V_hashtable_slot_type

    The following structs are generated for a given key type K and value type V:
    - K_to_V_hashtable_init
    - K_to_V_hashtable_init_internal
    - K_to_V_hashtable_init_with_capacity_rounded
    - K_to_V_hashtable_deinit
    - K_to_V_hashtable_copy
    - K_to_V_hashtable_get_count
    - K_to_V_hashtable_get_capacity
    - K_to_V_hashtable_is_empty
    - K_to_V_hashtable_is_full
    - K_to_V_hashtable_contains
    - K_to_V_hashtable_get
    - K_to_V_hashtable_get_mut
    - K_to_V_hashtable_set
    - K_to_V_hashtable_insert
    - K_to_V_hashtable_del

    Define PREFIX to use an other prefix than "K_to_V_hashtable". The macro(s) cannot
    however be redefined to have other prefixes.

    Note that (if PREFIX is not defined), then the given types cannot include
    spaces because C functions and variables cannot either.
    Use a typedef and replace spaces with _ or change the type name as needed.

    Some implementation details to consider, if `init_internal` is used:
    - The size must be a power of 2.
    - A larger hashtable than neccessary should be used to get the most performance
    of the open-addressing hash table, because performance of open-addressing hash
    tables worsens dramtically as the load factor (# of elements / size) approaches 1.
    - At least one slot in the hashtable is expected to be reserved - for the case of
    missing keys. Load factor should not exactly be equal to 1.

    References:
    https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/
    https://thenumb.at/Hashtables/#robin-hood-linear-probing
    https://github.com/rmind/rhashmap/blob/master/src/rhashmap.c
*/

#ifndef __K_TO_V_HASHTABLE__H
#define __K_TO_V_HASHTABLE__H

#include "is_pow2.h"    // is_pow2
#include "murmurhash.h" // murmurhash
#include "nextpow2.h"   // nextpow2
#include <assert.h>     // assert
#include <stdbool.h>    // bool, true, false
#include <stddef.h>     // offsetof
#include <stdint.h>     // SIZE_MAX
#include <stdlib.h>     // size_t, NULL, malloc, free
#include <string.h>     // memcpy

#define EMPTY_SLOT_OFFSET SIZE_MAX

#define K_to_V_hashtable_for_each(hashtable_p, index, key, value)      \
    for ((index) = 0; (index) <= (hashtable_p)->index_mask; (index)++) \
        if ((hashtable_p)->arr[(index)].offset != EMPTY_SLOT_OFFSET && \
            ((key) = (hashtable_p)->arr[(index)].key, (value) = (hashtable_p)->arr[(index)].value, true))

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

#ifndef PREFIX
#define K_to_V_hashtable JOIN(KEY_TYPE, JOIN(JOIN(to, VALUE_TYPE), hashtable))
#else
#define K_to_V_hashtable PREFIX
#endif
#define K_to_V_hashtable_type JOIN(K_to_V_hashtable, type)
#define K_to_V_hashtable_slot JOIN(K_to_V_hashtable, slot)
#define K_to_V_hashtable_slot_type JOIN(K_to_V_hashtable_slot, type)

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

    if (pow2_capacity > (SIZE_MAX - offsetof(K_to_V_hashtable_type, arr)) / sizeof(K_to_V_hashtable_slot_type)) {
        return false;
    }
    *hashtable_pp =
        (K_to_V_hashtable_type*)malloc(offsetof(K_to_V_hashtable_type, arr) + sizeof(K_to_V_hashtable_slot_type) * pow2_capacity);
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

static inline bool JOIN(K_to_V_hashtable, init_with_capacity_rounded)(K_to_V_hashtable_type** hashtable_pp, size_t capacity) {
    assert(hashtable_pp != NULL);

    if (capacity == 0) {
        return false;
    }
    size_t rounded_capacity = nextpow2(capacity);
    if (rounded_capacity < capacity) {
        return false;
    }

    return JOIN(K_to_V_hashtable, init_internal)(hashtable_pp, rounded_capacity);
}

static inline bool JOIN(K_to_V_hashtable, init)(K_to_V_hashtable_type** hashtable_pp, size_t capacity) {
    assert(hashtable_pp != NULL);

    return JOIN(K_to_V_hashtable, init_with_capacity_rounded)(hashtable_pp, (capacity == 1) + 1.5 * capacity);
}

static inline bool JOIN(K_to_V_hashtable, deinit)(K_to_V_hashtable_type** hashtable_pp) {
    assert(hashtable_pp != NULL);

    if (*hashtable_pp == NULL) {
        return false;
    }
    free(*hashtable_pp);
    *hashtable_pp = NULL;

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

static inline bool JOIN(K_to_V_hashtable, contains)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false &&
           "loops forever for missing keys. consider using a larger-sized hashtable.");
#undef K_to_V_hashtable_is_full

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET && max_possible_offset <= hashtable_p->arr[index].offset) {
        if (KEY_IS_EQUAL(hashtable_p->arr[index].key, key)) {
            return true;
        }
        index = (index + 1) & hashtable_p->index_mask;
        max_possible_offset++;
    }
    return false;
}

static inline VALUE_TYPE* JOIN(K_to_V_hashtable, get_mut)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false &&
           "loops forever for missing keys. consider using a larger-sized hashtable.");
#undef K_to_V_hashtable_is_full

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET && max_possible_offset <= hashtable_p->arr[index].offset) {
        if (KEY_IS_EQUAL(hashtable_p->arr[index].key, key)) {
            return &hashtable_p->arr[index].value;
        }
        index = (index + 1) & hashtable_p->index_mask;
        max_possible_offset++;
    }
    return NULL;
}

static inline VALUE_TYPE JOIN(K_to_V_hashtable, get)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key,
                                                     const VALUE_TYPE default_value) {
    assert(hashtable_p != NULL);

    VALUE_TYPE* value_p = JOIN(K_to_V_hashtable, get_mut)(hashtable_p, key);
    return value_p != NULL ? *value_p : default_value;
}

static inline void JOIN(K_to_V_hashtable, insert)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key, const VALUE_TYPE value) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false);
#undef K_to_V_hashtable_is_full
#define K_to_V_hashtable_contains JOIN(K_to_V_hashtable, contains)
    assert(K_to_V_hashtable_contains(hashtable_p, key) == false);
#undef K_to_V_hashtable_contains

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    K_to_V_hashtable_slot_type current_slot = {.offset = 0, .key = key, .value = value};

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET) {
        // swap if current offset is larger. will ensure the maximum
        // offset (from the ideal position) is minimized.
        if (current_slot.offset > hashtable_p->arr[index].offset) {
            K_to_V_hashtable_slot_type temp = hashtable_p->arr[index];
            hashtable_p->arr[index] = current_slot;
            current_slot = temp;
        }
        index = (index + 1) & hashtable_p->index_mask;
        current_slot.offset++;
    }
    hashtable_p->arr[index] = current_slot;
    hashtable_p->count++;
}

static inline void JOIN(K_to_V_hashtable, set)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key, const VALUE_TYPE value) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_full JOIN(K_to_V_hashtable, is_full)
    assert(K_to_V_hashtable_is_full(hashtable_p) == false);
#undef K_to_V_hashtable_is_full

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    K_to_V_hashtable_slot_type current_slot = {.offset = 0, .key = key, .value = value};

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET) {
        if (current_slot.offset == hashtable_p->arr[index].offset && KEY_IS_EQUAL(current_slot.key, hashtable_p->arr[index].key)) {
            hashtable_p->arr[index].value = current_slot.value;
            return;
        }
        // swap if current offset is larger. will ensure the maximum
        // offset (from the ideal position) is minimized.
        if (current_slot.offset > hashtable_p->arr[index].offset) {
            K_to_V_hashtable_slot_type temp = hashtable_p->arr[index];
            hashtable_p->arr[index] = current_slot;
            current_slot = temp;
        }
        index = (index + 1) & hashtable_p->index_mask;
        current_slot.offset++;
    }
    hashtable_p->arr[index] = current_slot;
    hashtable_p->count++;
}

static inline bool JOIN(K_to_V_hashtable, del)(K_to_V_hashtable_type* hashtable_p, const KEY_TYPE key) {
    assert(hashtable_p != NULL);
#define K_to_V_hashtable_is_empty JOIN(K_to_V_hashtable, is_empty)
    assert(K_to_V_hashtable_is_empty(hashtable_p) == false);
#undef K_to_V_hashtable_is_empty

    size_t key_hash = HASH_FUNCTION(key);
    size_t index = key_hash & hashtable_p->index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_p->arr[index].offset != EMPTY_SLOT_OFFSET && max_possible_offset <= hashtable_p->arr[index].offset) {
        if (KEY_IS_EQUAL(hashtable_p->arr[index].key, key)) {
            // mark as deleted:
            hashtable_p->arr[index].offset = EMPTY_SLOT_OFFSET;
            hashtable_p->count--;

            // reduce offsets as much as possible by moving back offset elements:
            size_t next_index = (index + 1) & hashtable_p->index_mask;
            while (hashtable_p->arr[next_index].offset != EMPTY_SLOT_OFFSET && hashtable_p->arr[next_index].offset > 0) {
                hashtable_p->arr[index] = hashtable_p->arr[next_index];
                hashtable_p->arr[index].offset--;
                hashtable_p->arr[next_index].offset = EMPTY_SLOT_OFFSET;

                index = next_index;
                next_index = (index + 1) & hashtable_p->index_mask;
            }
            return true;
        }
        index = (index + 1) & hashtable_p->index_mask;
        max_possible_offset++;
    }
    return false;
}

#undef K_to_V_hashtable
#undef K_to_V_hashtable_type
#undef K_to_V_hashtable_slot
#undef K_to_V_hashtable_slot_type

#undef PREFIX
#undef HASH_FUNCTION
#undef KEY_IS_EQUAL
#undef KEY_TYPE
#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN
