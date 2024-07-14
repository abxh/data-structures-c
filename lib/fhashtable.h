/**
 * @file fhashtable.h
 * @brief Fixed-size hashtable data structure based on open adressing (robin hood hashing).
 *
 * Ensure the capacity rounded up to the power of 2 is 75% of the expected numbers of values to be stored to keep
 * load factor low and the hash table performant. Also note, searching for missing keys in full hashtables makes the search
 * procedures loop indefinitely - so ensure the hashtable doesn't get full.
 *
 * Prefer to use scalar types (int/uint/pointers) or strings as key/value pairs. Structs can be used with elementwise
 * equality check but will not make use the cache and hardware prefetching as well. Keep the structs in a seperate
 * buffer and use their pointers preferably.
 *
 * A static / heap-allocated buffer should be used, should the key/values's lifetime extend beyond
 * the scope the arguments are provided. This applies to strings. See example.
 *
 * The following macros must be defined:
 *  @li `NAME`
 *  @li `KEY_TYPE`
 *  @li `VALUE_TYPE`
 *  @li `KEY_IS_EQUAL(a,b)`
 *  @li `HASH_FUNCTION(key)`
 *
 * Source(s) used for inspiration:
 *  @li https://thenumb.at/Hashtables/#robin-hood-linear-probing
 *  @li https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/
 */

/**
 * @example fhashtable/fhashtable.c
 * Example of how `fhashtable.h` header file is used in practice.
 */

/**
 * @example arena/fhashtable.c
 * Example of how `fhashtable.h` header file is used in practice with `arena.h`.
 */

#include "murmurhash.h"    // murmur_32
#include "paste.h"         // PASTE, XPASTE, JOIN
#include "round_up_pow2.h" // round_up_pow2

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// macro definitions: {{{

/**
 * @def NAME
 * @brief Prefix to queue type and operations.
 *
 * Is undefined after header is included.
 *
 * @attention This must be manually defined before including this header file.
 */
#ifndef NAME
#define NAME fhashtable
#error "Must define NAME."
#else
#define FHASHTABLE_NAME NAME
#endif

/**
 * @def KEY_TYPE()
 * @brief The key type. This must be manually defined before including this header file.
 *
 * Is undefined once header is included.
 */
#ifndef KEY_TYPE
#error "Must declare KEY_TYPE."
#define KEY_TYPE int
#endif

/**
 * @def VALUE_TYPE
 * @brief The value type. This must be manually defined before including this header file.
 *
 * Is undefined once header is included.
 */
#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE."
#define VALUE_TYPE int
#endif

/**
 * @def KEY_IS_EQUAL(a, b)
 * @brief Used to compare two keys. This must be manually defined before including this header file.
 *
 * Is undefined once header is included.
 *
 * @attention
 *   @li If comparing two scalar values, set this macro to ((a) == (b)).
 *   @li If comparing two strings, set this macro to strcmp() or strncmp() appropiately.
 *   @li If comparing two structs, set this macro to a function that does element-wise comparison between the structs.
 * @retval true If the two keys are equal. Equivalent to a non-zero int.
 * @retval false If the two key are not equal. Equivalent to the int 0.
 */
#ifndef KEY_IS_EQUAL
#error "Must define KEY_IS_EQUAL."
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#endif

/**
 * @def HASH_FUNCTION(key)
 * @brief Used to compute indicies of keys. This must be manually defined before including this header file.
 *
 * Is undefined once header is included.
 *
 * @param key The key.
 * @retval The hash of the key as `size_t`.
 */
#ifndef HASH_FUNCTION
#error "Must define HASH_FUNCTION."
#define HASH_FUNCTION(key) (murmur3_32((uint8_t*)&(key), sizeof(KEY_TYPE), 0))
#endif

#ifndef FHASHTABLE_H
/// @cond DO_NOT_DOCUMENT
#define FHASHTABLE_H
/// @endcond

/**
 * @def FHASHTABLE_EMPTY_SLOT_OFFSET
 * @brief Offset constant used to flag empty slots.
 */
#define FHASHTABLE_EMPTY_SLOT_OFFSET (SIZE_MAX)

/**
 * @def FHASHTABLE_FOREACH
 * @brief Iterate over the non-empty slots in the hashtable in arbitary order.
 * @warning Modifying the hashtable under the iteration may result in errors.
 *
 * temporary variables visible in scope:
 * @li _index
 *
 * @param[in] hashtable_ptr hashtable pointer.
 * @param[out] key_ Current key. Should be `KEY_TYPE`.
 * @param[out] value_ Current value. Should be `VALUE_TYPE`.
 */
#define FHASHTABLE_FOREACH(hashtable_ptr, key_, value_)                              \
    for (size_t _index = 0; _index < (hashtable_ptr)->capacity; _index++)            \
                                                                                     \
        if ((hashtable_ptr)->slots[_index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET && \
                                                                                     \
            ((key_) = (hashtable_ptr)->slots[_index].key, (value_) = (hashtable_ptr)->slots[_index].value, true))

#endif // FHASHTABLE_H

/// @cond DO_NOT_DOCUMENT
#define FHASHTABLE_TYPE JOIN(FHASHTABLE_NAME, type)
#define FHASHTABLE_SLOT_TYPE JOIN(JOIN(FHASHTABLE_NAME, slot), type)
#define FHASHTABLE_IS_FULL JOIN(FHASHTABLE_NAME, is_full)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @brief Generated hashtable slot struct type for a given `KEY_TYPE` and `VALUE_TYPE`.
 */
typedef struct {
    size_t offset;    ///< Offset from the ideal slot index.
    KEY_TYPE key;     ///< The key in this slot
    VALUE_TYPE value; ///< The value in this slot
} FHASHTABLE_SLOT_TYPE;

/**
 * @brief Generated hashtable struct type for a given `KEY_TYPE` and `VALUE_TYPE`.
 */
typedef struct {
    size_t count;                 ///< Number of non-empty slots.
    size_t capacity;              ///< Number of slots allocated for in the hashtable.
    FHASHTABLE_SLOT_TYPE slots[]; ///< Array of slots.
} FHASHTABLE_TYPE;

// }}}

// function definitions: {{{
/**
 * @brief Create an hashtable with a given capacity with malloc().
 *
 * @note Ensure the capacity rounded up to the power of 2 is 75% of the expected numbers of values to be stored to keep
 * load factor low and the hash table performant.
 *
 * @param[in] capacity Maximum number of elements expected to be stored in the queue.
 * @return A pointer to the queue.
 * @retval `NULL`
 *   @li If malloc fails.
 *   @li If capacity is less than 2 or the hashtable size [rounded up to the power of 2] is larger than UINT32_MAX / 4.
 */
static inline FHASHTABLE_TYPE* JOIN(FHASHTABLE_NAME, create)(const size_t capacity) {
    if (capacity < 2 || capacity > UINT32_MAX / 4) {
        return NULL;
    }
    const size_t capacity_new = round_up_pow2(capacity);
    if (capacity_new > (SIZE_MAX - offsetof(FHASHTABLE_TYPE, slots)) / sizeof(FHASHTABLE_SLOT_TYPE)) {
        return NULL;
    }

    FHASHTABLE_TYPE* hashtable_ptr =
        (FHASHTABLE_TYPE*)malloc(offsetof(FHASHTABLE_TYPE, slots) + capacity_new * sizeof(FHASHTABLE_SLOT_TYPE));
    if (!hashtable_ptr) {
        return NULL;
    }
    memset(hashtable_ptr, 0, offsetof(FHASHTABLE_TYPE, slots) + capacity_new * sizeof(FHASHTABLE_SLOT_TYPE));

    hashtable_ptr->count = 0;
    hashtable_ptr->capacity = capacity_new;

    for (size_t i = 0; i < hashtable_ptr->capacity; i++) {
        hashtable_ptr->slots[i].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;
    }

    return hashtable_ptr;
}

/**
 * @brief Destroy an hashtable and free the underlying memory with free().
 *
 * Assumes:
 * @li The hashtable pointer is not `NULL`.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @warning May not be called twice in a row on the same object.
 */
static inline void JOIN(FHASHTABLE_NAME, destroy)(FHASHTABLE_TYPE* hashtable_ptr) {
    free(hashtable_ptr);
}

/**
 * @brief Return whether the hashtable is empty.
 *
 * Assumes the hashtable pointer is not `NULL`.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @return whether the hashtable is empty.
 */
static inline bool JOIN(FHASHTABLE_NAME, is_empty)(const FHASHTABLE_TYPE* hashtable_ptr) {
    assert(hashtable_ptr != NULL);

    return hashtable_ptr->count == 0;
}

/**
 * @brief Return whether the hashtable is full.
 *
 * Assumes the hashtable pointer is not `NULL`.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @return whether the hashtable is full.
 */
static inline bool JOIN(FHASHTABLE_NAME, is_full)(const FHASHTABLE_TYPE* hashtable_ptr) {
    assert(hashtable_ptr != NULL);

    return hashtable_ptr->count == hashtable_ptr->capacity;
}

/**
 * @brief Check if hashtable contains a key.
 *
 * Assumes:
 * @li hashtable_ptr is not `NULL`.
 * @li there exists at least one empty slot - aka the hashtable is not full.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key.
 * @return A boolean indicating whether the hashtable contains the given key.
 */
static inline bool JOIN(FHASHTABLE_NAME, contains_key)(const FHASHTABLE_TYPE* hashtable_ptr, const KEY_TYPE key) {
    assert(hashtable_ptr != NULL);
    assert(FHASHTABLE_IS_FULL(hashtable_ptr) == false);

    const size_t key_hash = HASH_FUNCTION(key);
    const size_t index_mask = hashtable_ptr->capacity - 1;

    size_t index = key_hash & index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_ptr->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET &&

           max_possible_offset <= hashtable_ptr->slots[index].offset) {

        if (KEY_IS_EQUAL(hashtable_ptr->slots[index].key, key)) {
            return true;
        }

        index++;
        index &= index_mask;
        max_possible_offset++;
    }
    return false;
}

/**
 * @brief From a given key, get the pointer to the corresponding value in the hashtable.
 *
 * Assumes:
 * @li hashtable_ptr is not `NULL`.
 * @li there exists at least one empty slot - aka the hashtable is not full.
 *
 * @note The returned pointer is **not** garanteed to point to the same value if the hashtable
 * is modified.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key to search for.
 * @return A pointer to the corresponding key.
 *  @retval NULL If the hashtable did not contain the key.
 */
static inline VALUE_TYPE* JOIN(FHASHTABLE_NAME, get_value_mut)(FHASHTABLE_TYPE* hashtable_ptr, const KEY_TYPE key) {
    assert(hashtable_ptr != NULL);
    assert(FHASHTABLE_IS_FULL(hashtable_ptr) == false);

    const size_t key_hash = HASH_FUNCTION(key);
    const size_t index_mask = hashtable_ptr->capacity - 1;

    size_t index = key_hash & index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_ptr->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET &&

           max_possible_offset <= hashtable_ptr->slots[index].offset) {

        if (KEY_IS_EQUAL(hashtable_ptr->slots[index].key, key)) {
            return &hashtable_ptr->slots[index].value;
        }

        index++;
        index &= index_mask;
        max_possible_offset++;
    }
    return NULL;
}

/**
 * @brief From a given key, get the copy of the corresponding value in the hashtable.
 *
 * Assumes:
 * @li hashtable_ptr is not `NULL`.
 * @li there exists at least one empty slot - aka the hashtable is not full.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key to search for.
 * @param[in] default_value The default value returned if the hashtable did not contain the key.
 *
 * @return The corresponding key.
 *  @retval default_value If the hashtable did not contain the key.
 */
static inline VALUE_TYPE JOIN(FHASHTABLE_NAME, get_value)(const FHASHTABLE_TYPE* hashtable_ptr, const KEY_TYPE key,
                                                          const VALUE_TYPE default_value) {
    assert(hashtable_ptr != NULL);
    assert(FHASHTABLE_IS_FULL(hashtable_ptr) == false);

    const size_t key_hash = HASH_FUNCTION(key);
    const size_t index_mask = hashtable_ptr->capacity - 1;

    size_t index = key_hash & index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_ptr->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET &&

           max_possible_offset <= hashtable_ptr->slots[index].offset) {

        if (KEY_IS_EQUAL(hashtable_ptr->slots[index].key, key)) {
            return hashtable_ptr->slots[index].value;
        }

        index++;
        index &= index_mask;
        max_possible_offset++;
    }
    return default_value;
}

/**
 * @brief Search a given key in the hashtable and get the pointer to the corresponding value.
 *
 * Assumes hashtable_ptr is not `NULL`.
 *
 * The returned pointer is **not** garanteed to point to the same value if the hashtable
 * is modified.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key to search for.
 * @return A pointer to the corresponding key.
 * @retval NULL If the hashtable did not contain the key.
 */
static inline VALUE_TYPE* JOIN(FHASHTABLE_NAME, search)(FHASHTABLE_TYPE* hashtable_ptr, const KEY_TYPE key) {
    return JOIN(FHASHTABLE_NAME, get_value_mut)(hashtable_ptr, key);
}

/**
 * @brief Insert a non-duplicate key and it's corresponding value inside the hashtable.
 *
 * Assumes:
 * @li hashtable_ptr is not `NULL`.
 * @li key is not already contained in the hashtable.
 * @li hash table is not full.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key.
 * @param[in] value The value.
 * @return Whether the value is successfully inserted. 
 *  @retval true If the hashtable does *not* become full, once the value is inserted.
 *  @retval false If the hashtable does become full, once the value is inserted.
 */
static inline bool JOIN(FHASHTABLE_NAME, insert)(FHASHTABLE_TYPE* hashtable_ptr, KEY_TYPE key, VALUE_TYPE value) {
    assert(hashtable_ptr != NULL);
    assert(JOIN(FHASHTABLE_NAME, contains_key)(hashtable_ptr, key) == false);
    assert(FHASHTABLE_IS_FULL(hashtable_ptr) == false);

    if (hashtable_ptr->count + 1 == hashtable_ptr->capacity) {
        return false;
    }

    const size_t index_mask = hashtable_ptr->capacity - 1;
    const size_t key_hash = HASH_FUNCTION(key);

    size_t index = key_hash & index_mask;
    FHASHTABLE_SLOT_TYPE current_slot = {.offset = 0, .key = key, .value = value};

    while (hashtable_ptr->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET) {

        // swap if current offset is larger. will ensure the maximum
        // offset (from the ideal position) is minimized.

        if (current_slot.offset > hashtable_ptr->slots[index].offset) {

            FHASHTABLE_SLOT_TYPE temp = hashtable_ptr->slots[index];
            hashtable_ptr->slots[index] = current_slot;
            current_slot = temp;
        }
        index++;
        index &= index_mask;
        current_slot.offset++;
    }
    hashtable_ptr->slots[index] = current_slot;
    hashtable_ptr->count++;

    return true;
}

/**
 * @brief Update a key's corresponding value inside the hashtable. Allows duplicates.
 *
 * If a duplicate key is found, the corresponding value is overwritten.
 *
 * Assumes:
 * @li hashtable_ptr is not `NULL`.
 * @li hash table is not full.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key.
 * @param[in] value The value.
 * @return Whether the value is successfully inserted. 
 *  @retval true If the hashtable does *not* become full, once the value is inserted.
 *  @retval false If the hashtable does become full, once the value is inserted.
 */
static inline bool JOIN(FHASHTABLE_NAME, update)(FHASHTABLE_TYPE* hashtable_ptr, KEY_TYPE key, VALUE_TYPE value) {
    assert(hashtable_ptr != NULL);
    assert(FHASHTABLE_IS_FULL(hashtable_ptr) == false);

    const size_t index_mask = hashtable_ptr->capacity - 1;
    const size_t key_hash = HASH_FUNCTION(key);

    size_t index = key_hash & index_mask;
    FHASHTABLE_SLOT_TYPE current_slot = {.offset = 0, .key = key, .value = value};

    while (hashtable_ptr->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET) {

        if (current_slot.offset == hashtable_ptr->slots[index].offset &&

            KEY_IS_EQUAL(current_slot.key, hashtable_ptr->slots[index].key)) {

            hashtable_ptr->slots[index].value = current_slot.value;
            return true;
        }

        // swap if current offset is larger. will ensure the maximum
        // offset (from the ideal position) is minimized.

        if (current_slot.offset > hashtable_ptr->slots[index].offset) {

            FHASHTABLE_SLOT_TYPE temp = hashtable_ptr->slots[index];
            hashtable_ptr->slots[index] = current_slot;
            current_slot = temp;
        }

        index++;
        index &= index_mask;
        current_slot.offset++;
    }

    if (hashtable_ptr->count + 1 == hashtable_ptr->capacity) {
        return false;
    }

    hashtable_ptr->slots[index] = current_slot;
    hashtable_ptr->count++;

    return true;
}

/**
 * @brief Delete a key and it's corresponding value from the hashtable.
 *
 * Assumes hashtable_ptr is not `NULL`.
 *
 * @param[in] hashtable_ptr The hashtable pointer.
 * @param[in] key The key.
 * @return A boolean indicating whether the key was previously contained in the hashtable.
 */
static inline bool JOIN(FHASHTABLE_NAME, delete)(FHASHTABLE_TYPE* hashtable_ptr, const KEY_TYPE key) {
    assert(hashtable_ptr != NULL);

    const size_t index_mask = hashtable_ptr->capacity - 1;
    const size_t key_hash = HASH_FUNCTION(key);

    size_t index = key_hash & index_mask;
    size_t max_possible_offset = 0;

    while (hashtable_ptr->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET &&

           max_possible_offset <= hashtable_ptr->slots[index].offset) {

        if (KEY_IS_EQUAL(hashtable_ptr->slots[index].key, key)) {

            // mark as deleted:
            hashtable_ptr->slots[index].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;
            hashtable_ptr->count--;

            // reduce offsets as much as possible by moving back offset elements:

            size_t next_index = (index + 1) & index_mask;

            while (hashtable_ptr->slots[next_index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET &&

                   hashtable_ptr->slots[next_index].offset > 0) {

                hashtable_ptr->slots[index] = hashtable_ptr->slots[next_index];
                hashtable_ptr->slots[index].offset--;

                hashtable_ptr->slots[next_index].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;

                index = next_index;
                next_index = (index + 1) & index_mask;
            }
            return true;
        }
        index = (index + 1) & index_mask;
        max_possible_offset++;
    }
    return false;
}

/**
 * @brief Clear an existing hashtable and flag all slots as empty.
 * @param[in] hashtable_ptr The pointer of the hashtable to clear.
 */
static inline void JOIN(FHASHTABLE_NAME, clear)(FHASHTABLE_TYPE* hashtable_ptr) {
    assert(hashtable_ptr != NULL);

    for (size_t i = 0; i < hashtable_ptr->capacity; i++) {
        hashtable_ptr->slots[i].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;
    }
    hashtable_ptr->count = 0;
}

/**
 * @brief Copy the values from a source hashtable to a destination hashtable.
 *
 * Assumes:
 * @li Source and destination hashtable pointers are not pointing to the same memory.
 * @li The hashtable pointers are not `NULL`.
 * @li The destination hashtable has a capacity that is greater than or equal to source hashtable count.
 * @li The destination hashtable is an empty hashtable.
 *
 * @param[in] src_hashtable_ptr The source hashtable.
 * @param[out] dest_hashtable_ptr The destination hashtable.
 */
static inline void JOIN(FHASHTABLE_NAME, copy)(FHASHTABLE_TYPE* restrict dest_hashtable_ptr,
                                               const FHASHTABLE_TYPE* restrict src_hashtable_ptr) {
    assert(src_hashtable_ptr != NULL);
    assert(dest_hashtable_ptr != NULL);
    assert(src_hashtable_ptr->capacity <= dest_hashtable_ptr->capacity);
    assert(dest_hashtable_ptr->count == 0);

    for (size_t i = 0; i < src_hashtable_ptr->capacity; i++) {
        dest_hashtable_ptr->slots[i] = src_hashtable_ptr->slots[i];
    }

    dest_hashtable_ptr->count = src_hashtable_ptr->count;
}

// }}}

// macro undefs: {{{

#undef NAME
#undef KEY_TYPE
#undef VALUE_TYPE
#undef KEY_IS_EQUAL
#undef HASH_FUNCTION

#undef FHASHTABLE_SLOT_TYPE
#undef FHASHTABLE_TYPE
#undef FHASHTABLE_IS_FULL

// }}}

// vim: ft=c fdm=marker
