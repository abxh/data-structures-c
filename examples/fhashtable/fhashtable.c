#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

size_t first_char(const char* c) {
    return *c;
}
#define NAME strint_ht
#define KEY_TYPE char*
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) (strcmp((a), (b)) == 0)
#define HASH_FUNCTION(v) (first_char(v))
#include "fhashtable.h"

static char buf[4096]; // a static / heap-allocated buffer should be used, should the key/values's lifetime
                       // extend beyond the function scope [in the function below].

void str_int_ht_test(void) {

    strint_ht_type* ht = strint_ht_create(3);
    if (!ht) {
        assert(false);
    }
    assert(ht->count == 0);

    size_t buf_offset = 0;

    strcpy(&buf[buf_offset], "egg");
    char* egg_str_p = &buf[buf_offset];
    buf_offset += sizeof("egg");

    strint_ht_insert(ht, egg_str_p, 1);
    strint_ht_update(ht, egg_str_p, 2);

    strcpy(&buf[buf_offset], "milk");
    char* milk_str_p = &buf[buf_offset];
    buf_offset += sizeof("milk");

    strint_ht_update(ht, milk_str_p, 3);

    assert(ht->count == 2);

    assert(strint_ht_contains_key(ht, "milk"));
    assert(!strint_ht_contains_key(ht, "chocolate"));

    assert(strint_ht_get_value(ht, "egg", -1) == 2);

    int* res = strint_ht_get_value_mut(ht, "milk");
    if (res == NULL) {
        assert(false);
    }
    assert(*res == 3);
    *res = 4;

    assert(strint_ht_get_value(ht, "milk", -1) == 4);

    assert(strint_ht_delete(ht, "milk"));
    assert(!strint_ht_delete(ht, "chocolate"));

    assert(!strint_ht_contains_key(ht, "milk"));

    assert(strint_ht_search(ht, "milk") == NULL); // alias to get_mut

    assert(ht->count == 1);
    strint_ht_clear(ht);
    assert(ht->count == 0);

    assert(!strint_ht_contains_key(ht, "egg"));

    strint_ht_destroy(ht);
}

#include "fnvhash.h"

#define NAME int_to_int_hashtable
#define KEY_TYPE int
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#define HASH_FUNCTION(key) (fnvhash_32((uint8_t*)&(key), sizeof(int)))
#include "fhashtable.h"

#define LIM ((int)(1e+6))

void int_to_int_hashtable_test(void) {
    int_to_int_hashtable_type* ht = int_to_int_hashtable_create(LIM);
    if (!ht) {
        assert(false);
    }
    for (int i = 0; i < LIM; i++) {
        int_to_int_hashtable_insert(ht, i, LIM - i);
    }
    for (int i = 0; i < LIM; i++) {
        assert(int_to_int_hashtable_get_value(ht, i, -1) == LIM - i);
    }

    int_to_int_hashtable_type* ht_copy = int_to_int_hashtable_create(ht->count);
    if (!ht_copy) {
        assert(false);
    }
    int_to_int_hashtable_copy(ht_copy, ht);

    int_to_int_hashtable_destroy(ht);

    bool table[LIM] = {false};
    {
        size_t temp_index;
        int key;
        int value;
        (void)(value); // mark unused

        FHASHTABLE_FOREACH(ht_copy, temp_index, key, value) {
            const bool inside_range = 0 <= key && key < LIM;
            const bool already_counted = table[key];

            if (!inside_range || already_counted) {
                assert(false);
            }

            table[key] = true;
        }
    }

    for (size_t i = 0; i < LIM; i++) {
        assert(table[i] == true);
    }

    int_to_int_hashtable_destroy(ht_copy);
}

int main(void) {
    str_int_ht_test();
    int_to_int_hashtable_test();
    return 0;
}