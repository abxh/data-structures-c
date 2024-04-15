#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr
#include <stdlib.h>  // NULL
#include <string.h>  // strcmp

#include "test.h" // is_true, is_false, is_equal

#define PREFIX int_ht
#define KEY_TYPE int
#define VALUE_TYPE int
#include "fixed-containers/K_to_V_hashtable.h" // int_ht_*, K_to_V_hashtable_for_each

bool one_element_test(void) {
    int_ht_type* int_ht_p = NULL;
    if (!int_ht_init(&int_ht_p, 1)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_ht_get_count(int_ht_p), 0UL);
    int_ht_set(int_ht_p, 1, 42);
    res &= is_equal(int_ht_get_count(int_ht_p), 1UL);
    res &= is_equal(int_ht_get(int_ht_p, 1, -1), 42);

    res &= is_true(int_ht_deinit(&int_ht_p));

    return res;
}

#define PREFIX strint_ht
#define KEY_TYPE const char*
#define VALUE_TYPE int
#define KEY_IS_EQUAL(a, b) (strcmp((a), (b)) == 0)
#define HASH_FUNCTION(v) (*(v))
#include "fixed-containers/K_to_V_hashtable.h" // strint_ht_*

bool four_elements_test(void) {
    strint_ht_type* ht_p = NULL;
    if (!strint_ht_init(&ht_p, 4)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    size_t o = sizeof("AA");
    char* buf = malloc(4 * o);
    memcpy(&buf[0 * o], "AA", o);
    memcpy(&buf[1 * o], "AB", o);
    memcpy(&buf[2 * o], "BB", o);
    memcpy(&buf[3 * o], "CC", o);

    strint_ht_set(ht_p, &buf[0 * o], 1);
    strint_ht_set(ht_p, &buf[1 * o], 2);
    strint_ht_set(ht_p, &buf[2 * o], 3);
    strint_ht_set(ht_p, &buf[3 * o], 4);

    res &= is_equal(strint_ht_get_count(ht_p), 4UL);

    res &= is_equal(strint_ht_get(ht_p, "AA", -1), 1);
    res &= is_equal(strint_ht_get(ht_p, "AB", -1), 2);
    res &= is_equal(strint_ht_get(ht_p, "BB", -1), 3);
    res &= is_equal(strint_ht_get(ht_p, "CC", -1), 4);

    free(buf);
    res &= is_true(strint_ht_deinit(&ht_p));

    return res;
}

bool missing_elements_test(void) {
    int_ht_type* int_ht_p = NULL;
    if (!int_ht_init(&int_ht_p, 2)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_false(int_ht_contains(int_ht_p, 1));
    res &= is_false(int_ht_contains(int_ht_p, 2));
    res &= is_false(int_ht_contains(int_ht_p, 3));
    res &= is_false(int_ht_contains(int_ht_p, 4));

    int_ht_set(int_ht_p, 1, 42);
    int_ht_set(int_ht_p, 4, 69);

    res &= is_true(int_ht_contains(int_ht_p, 1));
    res &= is_false(int_ht_contains(int_ht_p, 2));
    res &= is_false(int_ht_contains(int_ht_p, 3));
    res &= is_true(int_ht_contains(int_ht_p, 4));

    res &= is_true(int_ht_deinit(&int_ht_p));

    return res;
}

bool del_and_contains_test(void) {
    strint_ht_type* ht_p = NULL;
    if (!strint_ht_init(&ht_p, 4)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }

    bool res = true;

    size_t o = sizeof("aaa");
    char* buf = malloc(4 * o);
    memcpy(&buf[0 * o], "aaa", o);
    memcpy(&buf[1 * o], "aab", o);
    memcpy(&buf[2 * o], "aac", o);
    memcpy(&buf[3 * o], "bbb", o);

    strint_ht_set(ht_p, &buf[0 * o], 1);
    strint_ht_set(ht_p, &buf[1 * o], 2);
    strint_ht_set(ht_p, &buf[2 * o], 3);
    strint_ht_set(ht_p, &buf[3 * o], 4);

    res &= is_equal(strint_ht_get_count(ht_p), 4UL);
    res &= is_true(strint_ht_del(ht_p, "aab"));
    res &= is_equal(strint_ht_get_count(ht_p), 3UL);

    res &= is_false(strint_ht_del(ht_p, "ccc"));

    res &= is_true(strint_ht_contains(ht_p, "aaa"));
    res &= is_false(strint_ht_contains(ht_p, "aab"));
    res &= is_true(strint_ht_contains(ht_p, "aac"));
    res &= is_true(strint_ht_contains(ht_p, "bbb"));

    res &= is_equal(strint_ht_get(ht_p, "aaa", -1), 1);
    res &= is_equal(strint_ht_get(ht_p, "aac", -1), 3);
    res &= is_equal(strint_ht_get(ht_p, "bbb", -1), 4);

    res &= is_true(strint_ht_del(ht_p, "aaa"));
    res &= is_false(strint_ht_del(ht_p, "aaa"));
    res &= is_equal(strint_ht_get_count(ht_p), 2UL);

    res &= is_false(strint_ht_contains(ht_p, "aaa"));
    res &= is_false(strint_ht_contains(ht_p, "aab"));
    res &= is_true(strint_ht_contains(ht_p, "aac"));
    res &= is_true(strint_ht_contains(ht_p, "bbb"));

    res &= is_equal(strint_ht_get(ht_p, "aac", -1), 3);
    res &= is_equal(strint_ht_get(ht_p, "bbb", -1), 4);

    res &= is_true(strint_ht_del(ht_p, "aac"));
    res &= is_false(strint_ht_del(ht_p, "aac"));

    res &= is_false(strint_ht_contains(ht_p, "aaa"));
    res &= is_false(strint_ht_contains(ht_p, "aab"));
    res &= is_false(strint_ht_contains(ht_p, "aac"));
    res &= is_true(strint_ht_contains(ht_p, "bbb"));

    res &= is_equal(strint_ht_get(ht_p, "bbb", -1), 4);

    res &= is_true(strint_ht_deinit(&ht_p));

    return res;
}

bool overwrite_element_test(void) {
    int_ht_type* ht_p = NULL;
    if (!int_ht_init(&ht_p, 2)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_ht_get_count(ht_p), 0UL);
    int_ht_set(ht_p, 1, 1111);
    res &= is_equal(int_ht_get_count(ht_p), 1UL);
    int_ht_set(ht_p, 1, 2222);
    res &= is_equal(int_ht_get_count(ht_p), 1UL);
    int_ht_set(ht_p, 1, 2222);
    res &= is_equal(int_ht_get_count(ht_p), 1UL);

    res &= is_equal(int_ht_get(ht_p, 1, -1), 2222);

    res &= is_true(int_ht_deinit(&ht_p));

    return res;
}

#define lim 1000000

bool million_elements_test(void) {
    int_ht_type* ht_p = NULL;
    if (!int_ht_init(&ht_p, lim)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    res &= is_equal(int_ht_get_count(ht_p), 0UL);
    for (int i = 0; i < lim; i++) {
        int_ht_set(ht_p, i, lim - i);
    }
    res &= is_equal(int_ht_get_count(ht_p), (size_t)lim);
    for (int i = 0; i < lim; i++) {
        res &= is_equal(int_ht_get(ht_p, i, -1), lim - i);
    }
    res &= is_true(int_ht_deinit(&ht_p));
    return res;
}

bool for_each_and_copy_test(void) {
    int_ht_type* ht_p = NULL;
    if (!int_ht_init(&ht_p, 1000)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    for (int i = 0; i < 1000; i++) {
        int_ht_set(ht_p, i, 1000 - i);
    }
    {
        size_t index;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
        int key;
        int value;
#pragma GCC diagnostic pop
        size_t count = 0;
        K_to_V_hashtable_for_each(ht_p, index, key, value) {
            count++;
        }
        res &= is_equal(1000UL, count);
    }
    return true;
    int_ht_type* ht_copy_p = NULL;
    if (!int_ht_copy(&ht_copy_p, ht_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    {
        size_t index;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
        int key;
        int value;
#pragma GCC diagnostic pop
        size_t count = 0;
        K_to_V_hashtable_for_each(ht_p, index, key, value) {
            count++;
            res &= is_true(value == int_ht_get(ht_p, key, -1));
        }
        res &= is_equal(1000UL, count);
    }
    res &= is_true(int_ht_deinit(&ht_p));
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} bool_f_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    bool_f_plus bool_f_arr[] = {{one_element_test, "one element test"},
                                {four_elements_test, "four elements test"},
                                {missing_elements_test, "missing elements test"},
                                {del_and_contains_test, "del and exists test"},
                                {overwrite_element_test, "overwrite element test"},
                                {million_elements_test, "million elements test"},
                                {for_each_and_copy_test, "for each and copy test"}};

    printf(__FILE_NAME__ ":\n");
    bool overall_res = true;
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        bool res = bool_f_arr[i].func();
        overall_res &= res;
        printf(" [%s] %s\n", res ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return overall_res ? 0 : 1;
}
