#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "arena.h"

size_t first_byte(const char* c) {
    return *c;
}
#define PREFIX strint_ht
#define KEY_TYPE const char*
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) (strcmp((a), (b)) == 0)
#define HASH_FUNCTION(v) (first_byte(v))
#include "hashtable.h"

static inline void* abort_if_null(void* ptr) {
    if (ptr == NULL) {
        // abort();
        assert("aborts here" && false);
    }
    return ptr;
}

void str_int_ht_test(void) {
    static unsigned char buf[2 * sizeof("egg") + sizeof("milk")]; // size can also just be some large enough number like 4096
    arena_type arena = {};
    arena_init(&arena, buf, sizeof(buf));

    strint_ht_type* ht = strint_ht_create();

    // insert over update for garanteed non-duplicate keys. more performant on optimized builds:
    assert(strint_ht_insert(ht, abort_if_null(arena_clone_string(&arena, "egg")), 1));

    assert(strint_ht_update(ht, abort_if_null(arena_clone_string(&arena, "egg")), 2));
    assert(strint_ht_update(ht, abort_if_null(arena_clone_string(&arena, "milk")), 3));

    assert(strint_ht_contains_key(ht, "milk"));
    assert(!strint_ht_contains_key(ht, "chocolate"));

    assert(strint_ht_get(ht, "egg", -1) == 2);

    int* res = strint_ht_get_mut(ht, "milk");
    if (res == NULL) {
        assert(false);
    }
    assert(*res == 3);
    *res = 4;

    assert(strint_ht_get(ht, "milk", -1) == 4);

    assert(strint_ht_delete(ht, "milk"));
    assert(!strint_ht_delete(ht, "chocolate"));

    assert(!strint_ht_contains_key(ht, "milk"));

    // alias to get_mut
    assert(strint_ht_search(ht, "milk") == NULL);

    strint_ht_destroy(ht);
}

#define KEY_TYPE int
#define VALUE_TYPE int
#include "hashtable.h"

#define lim ((size_t)1e+6)

void int_to_int_hashtable_test(void) {
    int_to_int_hashtable_type* ht = int_to_int_hashtable_create();
    if (!ht) {
        assert(false);
    }
    for (int i = 0; i < lim; i++) {
        assert(int_to_int_hashtable_insert(ht, i, lim - i) == true);
    }
    for (int i = 0; i < lim; i++) {
        assert(int_to_int_hashtable_get(ht, i, -1) == lim - i);
    }

    static bool table[lim];
    for (size_t i = 0; i < lim; i++) {
        table[i] = false;
    }

    {
        size_t index;
        int key;
        int value;
        hashtable_for_each(ht, index, key, value) {
            if (!(0 <= key && key < lim) || table[key]) {
                assert(false);
            }
            table[key] = true;
        }
    }

    for (size_t i = 0; i < lim; i++) {
        assert(table[i] == true);
    }

    int_to_int_hashtable_destroy(ht);
}

int main(void) {
    str_int_ht_test();
    int_to_int_hashtable_test();
    return 0;
}
