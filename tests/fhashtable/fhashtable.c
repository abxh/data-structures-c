// TDOO:
// more tests

/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 2
    - N := 16
    - N := 1e+3
    - N := 1e+9

    Non-mutating operation types / properties:
    - .count
    - .capacity
    - is_empty
    - is_full
    - contains_key + get_value + get_value_mut / search + fhashtable_for_each

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
    - scalar [numeric / pointers] => (==)
    - string => (strcmp) or (strncmp)
    - struct => (elementwise comparision)

    Hash functions:
    - Return same value
    - Bad custom hash function
    - murmurhash_32
    - fnvhash_32 / fnvhash_32_str

    Load ranges:
    - <25%
    - <50%
    - <75%
    - <100%
*/

#include <assert.h>

#include "fnvhash.h"
#include "murmurhash.h"

#define NAME int_to_int_ht
#define KEY_TYPE int
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#define HASH_FUNCTION(key) fnvhash_32((uint8_t*)&(key), sizeof(int))
#include "fhashtable.h"

#define check_duplicates(key_type, value_type, ht_p, n, key_exists_table, m, value_exists_table) \
    __extension__({                                                                              \
        assert((n) != 0);                                                                        \
        assert((m) != 0);                                                                        \
                                                                                                 \
        bool res = true;                                                                         \
        key_type key;                                                                            \
        value_type value;                                                                        \
                                                                                                 \
        size_t temp0;                                                                            \
        fhashtable_for_each(ht_p, temp0, key, value) {                                           \
            assert(key < (n));                                                                   \
            assert(value < (m));                                                                 \
            if (key_exists_table[key] == false) {                                                \
                key_exists_table[key] = true;                                                    \
            } else {                                                                             \
                res = false;                                                                     \
            }                                                                                    \
            if (value_exists_table[value] == false) {                                            \
                value_exists_table[value] = true;                                                \
            } else {                                                                             \
                res = false;                                                                     \
            }                                                                                    \
        }                                                                                        \
        res;                                                                                     \
    })

void int_int_full_test() {
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

        assert(ht_p->count == 0);
        assert(ht_p->capacity >= 2);
        assert(int_to_int_ht_is_empty(ht_p));
        assert(!int_to_int_ht_is_full(ht_p));

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

        assert(ht_p->count == 1);
        assert(ht_p->capacity >= 2);
        assert(!int_to_int_ht_is_empty(ht_p));
        assert(!int_to_int_ht_is_full(ht_p));

        bool key_exists[69 + 1] = {false};
        bool value_exists[69 + 1] = {false};

        int_to_int_ht_type* ht_copy_p = int_to_int_ht_create(2);
        if (!ht_copy_p) {
            assert(false);
        }
        int_to_int_ht_copy(ht_copy_p, ht_p);
        int_to_int_ht_destroy(ht_p);
        assert(check_duplicates(int, int, ht_copy_p, 70, key_exists, 70, value_exists));

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

        int_to_int_ht_destroy(ht_copy_p);
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
        int_to_int_ht_update(ht_p, 1375, 431);
        int_to_int_ht_update(ht_p, 0, 431);
        int_to_int_ht_update(ht_p, 555, 432);

        // replacement of existing values (+0):
        int_to_int_ht_update(ht_p, 71, 23);
        int_to_int_ht_update(ht_p, 0, 31);
        int_to_int_ht_update(ht_p, 555, 32);

        // check replacement works:
        assert(*int_to_int_ht_get_value_mut(ht_p, 71) == 23);
        assert(*int_to_int_ht_get_value_mut(ht_p, 0) == 31);
        assert(*int_to_int_ht_get_value_mut(ht_p, 555) == 32);

        // deletion of existing values (-3):
        assert(int_to_int_ht_delete(ht_p, 71));
        assert(int_to_int_ht_delete(ht_p, 1302));
        assert(int_to_int_ht_delete(ht_p, 555));

        // deletion of non-existing values:
        assert(!int_to_int_ht_delete(ht_p, 2));
        assert(!int_to_int_ht_delete(ht_p, 3));
        assert(!int_to_int_ht_delete(ht_p, 4));

        // check that values are actually deleted:
        assert(!int_to_int_ht_contains_key(ht_p, 71));
        assert(!int_to_int_ht_contains_key(ht_p, 130));
        assert(!int_to_int_ht_contains_key(ht_p, 555));

        // reinsert previous values and others (+5):
        int_to_int_ht_insert(ht_p, 555, 555);
        int_to_int_ht_insert(ht_p, 2, 3);
        int_to_int_ht_insert(ht_p, 71, 556);
        int_to_int_ht_insert(ht_p, 3, 4);
        int_to_int_ht_insert(ht_p, 4, 2);

        // check reinserted previous values
        assert(*int_to_int_ht_get_value_mut(ht_p, 555) == 555);
        assert(*int_to_int_ht_get_value_mut(ht_p, 71) == 556);

        assert(ht_p->count == 12);
        assert(ht_p->capacity >= 15);
        assert(!int_to_int_ht_is_empty(ht_p));
        assert(!int_to_int_ht_is_full(ht_p));

        bool* key_exists = calloc(2000, sizeof(bool));
        bool* value_exists = calloc(2000, sizeof(bool));
        int_to_int_ht_type* ht_copy_p = int_to_int_ht_create(16);
        if (!ht_copy_p) {
            assert(false);
        }
        int_to_int_ht_copy(ht_copy_p, ht_p);
        int_to_int_ht_destroy(ht_p);
        assert(check_duplicates(int, int, ht_copy_p, 2000, key_exists, 2000, value_exists));

        // assert that the correct keys / values exists:
        for (size_t i = 0; i < 2000; i++) {
            if (i == 421 || i == 422 || i == 423 || i == 425 || i == 426 || i == 431 || i == 31 || i == 555 || i == 3 || i == 556 ||
                i == 4 || i == 2) {
                assert(value_exists[i]);
            } else if (i == 20 || i == 1 || i == 53 || i == 71 || i == 113 || i == 902 || i == 1375 || i == 0 || i == 555 || i == 2 ||
                       i == 3 || i == 4) {
                assert(key_exists[i]);
            } else {
                assert(!value_exists[i]);
                assert(!key_exists[i]);
            }
        }

        free(key_exists);
        free(value_exists);
        int_to_int_ht_destroy(ht_copy_p);
    }
    // N = 1e+3, insert 1e+3, clear(), delete 2 * 1e+3, update 1e+2
    {
        int_to_int_ht_type* ht_p = int_to_int_ht_create(1e+3);
        if (!ht_p) {
            assert(false);
        }
        for (int i = 0; i < 1e+3; i++) {
            int_to_int_ht_insert(ht_p, 1e+6 - 690 - i, 1e+6 - i);
        }
        int_to_int_ht_clear(ht_p);
        for (int i = -1e+3; i < 1e+3; i++) {
            int_to_int_ht_delete(ht_p, 1e+6 - 690 - i);
        }
        for (int i = 0; i < 1e+2; i++) {
            int_to_int_ht_update(ht_p, 1e+6 - 690 - i, 1e+6 - 420 - i);
        }

        for (int i = 0; i < 1e+2; i++) {
            assert(*int_to_int_ht_get_value_mut(ht_p, 1e+6 - 690 - i) == 1e+6 - 420 - i);
        }
        for (int i = 1e+2; i < 1e+3; i++) {
            assert(int_to_int_ht_get_value_mut(ht_p, 1e+6 - 690 - i) == NULL);
        }

        assert(ht_p->count == 1e+2);
        assert(ht_p->capacity >= 1e+3);
        assert(!int_to_int_ht_is_empty(ht_p));
        assert(!int_to_int_ht_is_full(ht_p));

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

        assert(ht_p->count == 1e+6 - 500);
        assert(ht_p->capacity >= 1e+6);
        assert(!int_to_int_ht_is_empty(ht_p));
        assert(!int_to_int_ht_is_full(ht_p));

        int_to_int_ht_destroy(ht_p);
    }
}

#define NAME bd_ht
#define KEY_TYPE char*
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) (strcmp(a, b))
#define HASH_FUNCTION(key) (0)
#include "fhashtable.h"

void bad_hash_func_test() {
    // N = 1000, insert 1000, delete 100, update 100
    {
        bd_ht_type* bd_ht_p = bd_ht_create(1000 + 1);
        if (!bd_ht_p) {
            assert(false);
        }
    }
}

static inline size_t first_char(const char* c) {
    return *c;
}

#define NAME ch_ht
#define KEY_TYPE char*
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) (strcmp(a, b))
#define HASH_FUNCTION(key) (first_char(key))
#include "fhashtable.h"

typedef struct {
    int foo;
    int bar;
} a_struct;

static inline bool a_struct_eq(a_struct a, a_struct b) {
    return a.bar == b.bar && a.foo == b.foo;
}

#define NAME a_ht
#define KEY_TYPE a_struct
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) (a_struct_eq(a, b))
#define HASH_FUNCTION(key) (murmur3_32((uint8_t*)&key, sizeof(a_struct), 0))
#include "fhashtable.h"

int main(void) {
    int_int_full_test();
}
