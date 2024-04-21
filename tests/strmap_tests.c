#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr
#include <stdlib.h>  // NULL
#include <string.h>  // strcmp

#include "allocators/arena.h" // arena_*
#include "strmap.h"           // strmap_*, STRMAP_GET_VALUE_DEFAULT
#include "test.h"             // is_true, is_false, is_equal

bool one_element_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(strmap_get_count(strmap_p), 0UL);
    res &= is_true(strmap_set(strmap_p, "AA", "11"));
    res &= is_equal(strmap_get_count(strmap_p), 1UL);
    res &= is_true(strcmp(strmap_get(strmap_p, "AA"), "11") == 0);

    strmap_destroy(strmap_p);

    return res;
}

bool four_elements_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_true(strmap_set(strmap_p, "AAA", "1234"));
    res &= is_true(strmap_set(strmap_p, "BBB", "5678"));
    res &= is_true(strmap_set(strmap_p, "CCC", "9012"));
    res &= is_true(strmap_set(strmap_p, "DDD", "3456"));

    res &= is_true(strcmp(strmap_get(strmap_p, "AAA"), "1234") == 0);
    res &= is_true(strcmp(strmap_get(strmap_p, "BBB"), "5678") == 0);
    res &= is_true(strcmp(strmap_get(strmap_p, "CCC"), "9012") == 0);
    res &= is_true(strcmp(strmap_get(strmap_p, "DDD"), "3456") == 0);

    strmap_destroy(strmap_p);

    return res;
}

bool del_and_contains_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_true(strmap_set(strmap_p, "aaa", "1234"));
    res &= is_true(strmap_set(strmap_p, "aab", "5678"));
    res &= is_true(strmap_set(strmap_p, "aac", "9012"));
    res &= is_true(strmap_set(strmap_p, "aad", "3456"));

    res &= is_equal(strmap_get_count(strmap_p), 4UL);
    res &= is_true(strmap_del(strmap_p, "aab"));
    res &= is_equal(strmap_get_count(strmap_p), 3UL);

    res &= is_false(strmap_del(strmap_p, "aae"));

    res &= is_true(strmap_contains(strmap_p, "aaa"));
    res &= is_false(strmap_contains(strmap_p, "aab"));
    res &= is_true(strmap_contains(strmap_p, "aac"));
    res &= is_true(strmap_contains(strmap_p, "aad"));

    strmap_destroy(strmap_p);

    return res;
}

bool missing_elements_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_false(strmap_contains(strmap_p, "a"));
    res &= is_false(strmap_contains(strmap_p, "b"));
    res &= is_false(strmap_contains(strmap_p, "c"));
    res &= is_false(strmap_contains(strmap_p, "d"));

    res &= is_true(strmap_set(strmap_p, "a", "1234"));
    res &= is_true(strmap_set(strmap_p, "d", "3456"));

    res &= is_true(strmap_contains(strmap_p, "a"));
    res &= is_false(strmap_contains(strmap_p, "b"));
    res &= is_false(strmap_contains(strmap_p, "c"));
    res &= is_true(strmap_contains(strmap_p, "d"));

    strmap_destroy(strmap_p);

    return res;
}

bool overwrite_element_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(strmap_get_count(strmap_p), 0UL);

    res &= is_true(strmap_set(strmap_p, "A", "B"));
    res &= is_equal(strmap_get_count(strmap_p), 1UL);

    res &= is_true(strmap_set(strmap_p, "A", "C"));
    res &= is_equal(strmap_get_count(strmap_p), 1UL);

    res &= is_true(strmap_set(strmap_p, "A", "C"));
    res &= is_equal(strmap_get_count(strmap_p), 1UL);

    res &= is_true(strcmp(strmap_get(strmap_p, "A"), "C") == 0);

    strmap_destroy(strmap_p);

    return res;
}

bool empty_element_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_true(strmap_set(strmap_p, "A", ""));
    res &= is_true(strcmp(strmap_get(strmap_p, "A"), "") == 0);
    res &= is_true(strmap_set(strmap_p, "", "B"));
    res &= is_true(strcmp(strmap_get(strmap_p, ""), "B") == 0);
    res &= is_true(strmap_set(strmap_p, "", ""));
    res &= is_true(strcmp(strmap_get(strmap_p, ""), "") == 0);

    strmap_destroy(strmap_p);

    return res;
}

#define lim 1000000UL
#define m ('z' - 'a' + 1)

bool million_elements_test(void) {

    static unsigned char buf[300000 * 1024];
    arena_type arena = {0};
    arena_init(&arena, buf, sizeof(buf));

    strmap_type* strmap_p = NULL;
    if (!strmap_init(&strmap_p, &arena, arena_allocate, arena_reallocate, arena_deallocate)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    res &= is_equal(strmap_get_count(strmap_p), 0UL);
    for (size_t i = 0; i < lim; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        char c4 = (i / m / m / m) % m + 'a';
        char c5 = (i / m / m / m / m) % m + 'a';
        res &= is_true(strmap_set(strmap_p, (char[]){c5, c4, c3, c2, c1, '\0'}, (char[]){c1, c2, c3, c4, c5, '\0'}));
    }
    res &= is_equal(strmap_get_count(strmap_p), lim);
    for (size_t i = 0; i < lim; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        char c4 = (i / m / m / m) % m + 'a';
        char c5 = (i / m / m / m / m) % m + 'a';
        res &= is_true(strcmp(strmap_get(strmap_p, (char[]){c5, c4, c3, c2, c1, '\0'}), (char[]){c1, c2, c3, c4, c5, '\0'}) == 0);
    }
    strmap_destroy(strmap_p);
    return res;
}

bool for_each_and_copy_test(void) {
    strmap_type* strmap_p = strmap_create();
    if (!strmap_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    for (size_t i = 0; i < 1000; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        res &= is_true(strmap_set(strmap_p, (char[]){c3, c2, c1, '\0'}, (char[]){c1, c2, c3, '\0'}));
    }
    res &= is_equal(strmap_get_count(strmap_p), 1000UL);
    {
        size_t list_index;
        strmap_node_type* next_p = NULL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
        char* key_p = NULL;
        char* value_p = NULL;
#pragma GCC diagnostic pop
        size_t count = 0;
        strmap_for_each(strmap_p, list_index, next_p, key_p, value_p) {
            count++;
        }
        res &= is_equal(count, 1000UL);
    }
    strmap_type* strmap_copy_p = strmap_clone(strmap_p);
    if (!strmap_copy_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    {
        size_t list_index;
        strmap_node_type* next_p = NULL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
        char* key_p = NULL;
        char* value_p = NULL;
#pragma GCC diagnostic pop
        size_t i = 0;
        strmap_for_each(strmap_copy_p, list_index, next_p, key_p, value_p) {
            i++;
            res &= is_true(value_p != strmap_get(strmap_p, key_p));
            res &= is_true(strcmp(value_p, strmap_get(strmap_p, key_p)) == 0);
        }
        res &= is_equal(i, 1000UL);
    }

    strmap_destroy(strmap_p);
    strmap_destroy(strmap_copy_p);

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
                                {del_and_contains_test, "delete and contains test"},
                                {overwrite_element_test, "overwrite element test"},
                                {empty_element_test, "empty element test"},
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
