#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "arena.h"

#include "fnvhash.h"
#define NAME               strint_ht
#define KEY_TYPE           char*
#define VALUE_TYPE         int
#define KEY_IS_EQUAL(a, b) (strcmp((a), (b)) == 0)
#define HASH_FUNCTION(key) (fnvhash_32_str(key))
#include "fhashtable.h"

static unsigned char buf[4096];

void str_int_ht_test_alt(void)
{
    strint_ht_type* ht = strint_ht_create(3 * 4 / 3);
    if (!ht) {
        assert(false);
    }
    assert(ht->count == 0);

    arena_type arena;
    arena_init(&arena, sizeof(buf), buf); /* note that the arena also considers alignment details
                                             and can be used to store different types simultaneously. */

    char* egg_str_ptr = arena_allocate_aligned(&arena, alignof(char), sizeof("egg"));
    if (!egg_str_ptr) {
        assert(false);
    }
    strcpy(egg_str_ptr, "egg");

    strint_ht_insert(ht, egg_str_ptr, 1);
    strint_ht_update(ht, egg_str_ptr, 2);

    char* milk_str_ptr = arena_allocate(&arena, sizeof("egg"));
    if (!milk_str_ptr) {
        assert(false);
    }
    strcpy(milk_str_ptr, "milk");

    strint_ht_update(ht, milk_str_ptr, 3);

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
