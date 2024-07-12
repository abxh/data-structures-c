// TDOO:
// more tests

/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 2
    - N := 16
    - N := 1e+9

    Non-mutating operation types / properties:
    - .count
    - .capacity
    - is_empty
    - is_full
    - contains_key + get_value + get_value_mut / search + FHASHTABLE_FOREACH

    Mutating operation types:
    - insert
    - update
    - delete
    - clear

    Memory operations [to also be tested with sanitizers]:
    - create
    - destroy
    - copy

    Key / value types => KEY_IS_EQUAL():
    - scalar => (==)
    - pointer => (==)
    - string => (strcmp) or (strncmp)
    - struct => (elementwise comparision) [THIS IS NOT TESTED]

    Typical key / value combinations [to be tested]:
    - int to int
    - int to void*
    - int to const char*
    - const char* to int
    - const char* to void*
    - const char* to const char*
    - struct pointer to int
    - struct pointer to void*
    - struct pointer to const char*

    Hash functions:
    - Return same value + Bad custom hash function [Edge cases]
    - murmurhash_32 / fnvhash_32 [Good hash functions]

    Load ranges:
    - <25%
    - <50%
    - <75%
    - <100%
*/

#include <assert.h>

#include "fnvhash.h"

#define NAME int_to_int_ht
#define KEY_TYPE int
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#define HASH_FUNCTION(key) fnvhash_32((uint8_t*)&(key), sizeof(int))
#include "fhashtable.h"

#define check_count_invariance(ht_p, insert_count, delete_count) ((ht_p)->count == ((insert_count) - (delete_count)))

#define check_capacity_invariance(ht_p, old_capacity) ((ht_p)->capacity >= (old_capacity))

#define check_empty_full(name, ht_p, insert_count, delete_count) \
    __extension__({                                              \
        const size_t diff = (insert_count) - (delete_count);     \
        bool res = true;                                         \
        if (name##_is_empty(ht_p)) {                             \
            res = diff == 0;                                     \
        } else if (name##_is_full(ht_p)) {                       \
            res = diff == ht_p->capacity;                        \
        }                                                        \
        res;                                                     \
    })

#define check_duplicates(key_type, value_type, ht_p, n, key_exists_table, m, value_exists_table) \
    __extension__({                                                                              \
        assert((n) != 0);                                                                        \
        assert((m) != 0);                                                                        \
                                                                                                 \
        bool res = true;                                                                         \
        key_type key;                                                                            \
        value_type value;                                                                        \
        FHASHTABLE_FOREACH(ht_p, key, value) {                                                   \
            assert(key < n);                                                                     \
            assert(value < m);                                                                   \
            if (value_exists_table[value] == false) {                                            \
                value_exists_table[value] = true;                                                \
            } else {                                                                             \
                res = false;                                                                     \
            }                                                                                    \
            if (key_exists_table[key] == false) {                                                \
                key_exists_table[key] = true;                                                    \
            } else {                                                                             \
                res = false;                                                                     \
            }                                                                                    \
        }                                                                                        \
        res;                                                                                     \
    })

void int_to_int_ht__basic_test() {
    // N = 0
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(0);
        if (ht_p) {
            assert(false);
        }
    }
    // N = 1
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(1);
        if (ht_p) {
            assert(false);
        }
    }
    // N = 2
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(2);
        if (!ht_p) {
            assert(false);
        }

        assert(!int_to_int_ht_contains_key(ht_p, 42));
        assert(!int_to_int_ht_contains_key(ht_p, 69));
        assert(int_to_int_ht_get_value(ht_p, 42, -1) == -1);
        assert(int_to_int_ht_get_value(ht_p, 69, -1) == -1);
        assert(int_to_int_ht_get_value_mut(ht_p, 42) == NULL);
        assert(int_to_int_ht_get_value_mut(ht_p, 69) == NULL);

        assert(check_count_invariance(ht_p, 0, 0));
        assert(check_capacity_invariance(ht_p, 1));
        assert(check_empty_full(int_to_int_ht, ht_p, 0, 0));

        int_to_int_ht_destroy(ht_p);
    }
    // N = 2, insert
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(2);
        if (!ht_p) {
            assert(false);
        }
        int_to_int_ht_insert(ht_p, 42, 69);

        assert(int_to_int_ht_contains_key(ht_p, 42));
        assert(int_to_int_ht_get_value(ht_p, 42, -1) == 69);
        assert(*int_to_int_ht_get_value_mut(ht_p, 42) == 69);

        assert(!int_to_int_ht_contains_key(ht_p, 69));
        assert(int_to_int_ht_get_value(ht_p, 69, -1) == -1);
        assert(int_to_int_ht_get_value_mut(ht_p, 69) == NULL);

        assert(check_count_invariance(ht_p, 1, 0));
        assert(check_capacity_invariance(ht_p, 1));
        assert(check_empty_full(int_to_int_ht, ht_p, 1, 0));

        bool key_exists[69 + 1] = {false};
        bool value_exists[69 + 1] = {false};
        assert(check_duplicates(int, int, ht_p, 70, key_exists, 70, value_exists));

        // assert that the correct keys / values exists:
        for (size_t i = 0; i < 70; i++) {
            if (i == 69) {
                assert(value_exists[i]);
            } else if (i == 42) {
                assert(key_exists[i]);
            } else {
                assert(!value_exists[i]);
                assert(!key_exists[i]);
            }
        }

        int_to_int_ht_destroy(ht_p);
    }
    // N = 16, insert * 10 -> update * 7 -> del * 6 -> insert * 5
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(16);
        if (!ht_p) {
            assert(false);
        }
        // insert values (+10):
        int_to_int_ht_insert(ht_p, 20, 421);
        int_to_int_ht_insert(ht_p, 1, 422);
        int_to_int_ht_insert(ht_p, 53, 423);

        int_to_int_ht_insert(ht_p, 71, 424);
        int_to_int_ht_insert(ht_p, 113, 425);
        int_to_int_ht_insert(ht_p, 902, 426);

        int_to_int_ht_insert(ht_p, 1302, 429);
        int_to_int_ht_update(ht_p, 9999, 431);
        int_to_int_ht_update(ht_p, 0, 431);
        int_to_int_ht_update(ht_p, -555, 432);

        // replacement of existing values (+0):
        int_to_int_ht_update(ht_p, 71, 23);
        int_to_int_ht_update(ht_p, 0, 31);
        int_to_int_ht_update(ht_p, -555, 32);

        // check replacement works:
        assert(*int_to_int_ht_get_value_mut(ht_p, 71) == 23);
        assert(*int_to_int_ht_get_value_mut(ht_p, 0) == 31);
        assert(*int_to_int_ht_get_value_mut(ht_p, -555) == 32);

        // deletion of existing values (-3):
        assert(int_to_int_ht_delete(ht_p, 71));
        assert(int_to_int_ht_delete(ht_p, 1302));
        assert(int_to_int_ht_delete(ht_p, -555));

        // deletion of non-existing values:
        assert(!int_to_int_ht_delete(ht_p, 2));
        assert(!int_to_int_ht_delete(ht_p, 3));
        assert(!int_to_int_ht_delete(ht_p, 4));

        // check that values are actually deleted:
        assert(!int_to_int_ht_contains_key(ht_p, 71));
        assert(!int_to_int_ht_contains_key(ht_p, 130));
        assert(!int_to_int_ht_contains_key(ht_p, -555));

        // reinsert previous values and others (+5):
        int_to_int_ht_insert(ht_p, -1, 2);
        int_to_int_ht_insert(ht_p, -555, -555);
        int_to_int_ht_insert(ht_p, 2, 3);
        int_to_int_ht_insert(ht_p, 71, -555);
        int_to_int_ht_insert(ht_p, 3, 4);

        // check reinserted previous values
        assert(*int_to_int_ht_get_value_mut(ht_p, -555) == -555);
        assert(*int_to_int_ht_get_value_mut(ht_p, 71) == -555);

        assert(check_count_invariance(ht_p, 15, 3));
        assert(check_capacity_invariance(ht_p, 15));
        assert(check_empty_full(int_to_int_ht, ht_p, 15, 3));

        int_to_int_ht_destroy(ht_p);
    }
    // N = 1e+6, insert 1e+6, delete 10000, update 10000
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(1e+6);
        if (!ht_p) {
            assert(false);
        }
        for (int i = 0; i < 1e+6; i++) {
            int_to_int_ht_insert(ht_p, 1e+6 - 420 - i, 1e+6 - i);
        }
        for (int i = 0; i < 10000; i++) {
            int_to_int_ht_delete(ht_p, 1e+6 - 420 - i);
        }
        for (int i = 500; i < 10500; i++) {
            int_to_int_ht_update(ht_p, 1e+6 - 420 - i, 1e+6 - i);
        }

        for (int i = 0; i < 500; i++) {
            assert(int_to_int_ht_get_value_mut(ht_p, 1e+6 - 420 - i) == NULL);
        }
        for (int i = 500; i < 1e+6; i++) {
            assert(*int_to_int_ht_get_value_mut(ht_p, 1e+6 - 420 - i) == 1e+6 - i);
        }

        int_to_int_ht_destroy(ht_p);
    }
}

int main(void) {
    int_to_int_ht__basic_test();
}
