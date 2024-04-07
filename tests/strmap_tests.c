#include "assert.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include <stdio.h> // fprintf

#include "test.h"

#include "../src/strmap/strmap.h"

bool one_element_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(strmap_count(strmap_p), (size_t)0);
    res &= is_true(strmap_set(strmap_p, "AA", "11"));
    res &= is_equal(strmap_count(strmap_p), (size_t)1);
    res &= is_equal(strcmp(strmap_get(strmap_p, "AA"), "11"), 0);
    res &= is_true(strmap_deinit(&strmap_p));
    return res;
}

bool four_elements_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_true(strmap_set(strmap_p, "AAA", "1234"));
    res &= is_true(strmap_set(strmap_p, "BBB", "5678"));
    res &= is_true(strmap_set(strmap_p, "CCC", "9012"));
    res &= is_true(strmap_set(strmap_p, "DDD", "3456"));

    res &= is_equal(strcmp(strmap_get(strmap_p, "AAA"), "1234"), 0);
    res &= is_equal(strcmp(strmap_get(strmap_p, "BBB"), "5678"), 0);
    res &= is_equal(strcmp(strmap_get(strmap_p, "CCC"), "9012"), 0);
    res &= is_equal(strcmp(strmap_get(strmap_p, "DDD"), "3456"), 0);

    res &= is_true(strmap_deinit(&strmap_p));

    return res;
}

bool del_and_exists_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_true(strmap_set(strmap_p, "aaa", "1234"));
    res &= is_true(strmap_set(strmap_p, "aab", "5678"));
    res &= is_true(strmap_set(strmap_p, "aac", "9012"));
    res &= is_true(strmap_set(strmap_p, "aad", "3456"));

    res &= is_equal(strmap_count(strmap_p), (size_t)4);
    res &= is_true(strmap_del(strmap_p, "aab"));
    res &= is_equal(strmap_count(strmap_p), (size_t)3);

    res &= is_true(!strmap_del(strmap_p, "aae"));

    res &= is_true(strmap_exists(strmap_p, "aaa"));
    res &= is_false(strmap_exists(strmap_p, "aab"));
    res &= is_true(strmap_exists(strmap_p, "aac"));
    res &= is_true(strmap_exists(strmap_p, "aad"));

    res &= is_true(strmap_deinit(&strmap_p));

    return res;
}

bool missing_elements_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_false(strmap_exists(strmap_p, "a"));
    res &= is_false(strmap_exists(strmap_p, "b"));
    res &= is_false(strmap_exists(strmap_p, "c"));
    res &= is_false(strmap_exists(strmap_p, "d"));

    res &= is_true(strmap_set(strmap_p, "a", "1234"));
    res &= is_true(strmap_set(strmap_p, "d", "3456"));

    res &= is_true(strmap_exists(strmap_p, "a"));
    res &= is_false(strmap_exists(strmap_p, "b"));
    res &= is_false(strmap_exists(strmap_p, "c"));
    res &= is_true(strmap_exists(strmap_p, "d"));

    res &= is_true(strmap_deinit(&strmap_p));

    return res;
}

bool overwrite_element_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_equal(strmap_count(strmap_p), (size_t)0);

    res &= is_true(strmap_set(strmap_p, "A", "B"));
    res &= is_equal(strmap_count(strmap_p), (size_t)1);

    res &= is_true(strmap_set(strmap_p, "A", "C"));
    res &= is_equal(strmap_count(strmap_p), (size_t)1);

    res &= is_true(strmap_set(strmap_p, "A", "C"));
    res &= is_equal(strmap_count(strmap_p), (size_t)1);

    res &= is_equal(strcmp(strmap_get(strmap_p, "A"), "C"), 0);

    res &= is_true(strmap_deinit(&strmap_p));

    return res;
}

bool empty_element_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= is_true(strmap_set(strmap_p, "A", ""));
    res &= is_equal(strcmp(strmap_get(strmap_p, "A"), ""), 0);
    res &= is_true(strmap_set(strmap_p, "", "B"));
    res &= is_equal(strcmp(strmap_get(strmap_p, ""), "B"), 0);
    res &= is_true(strmap_set(strmap_p, "", ""));
    res &= is_equal(strcmp(strmap_get(strmap_p, ""), ""), 0);

    res &= is_true(strmap_deinit(&strmap_p));

    return res;
}

#define lim 1000000
#define m ('z' - 'a' + 1)

bool million_elements_test(void) {
    strmap_type* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    res &= is_equal(strmap_count(strmap_p), (size_t)0);
    for (size_t i = 0; i < lim; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        char c4 = (i / m / m / m) % m + 'a';
        char c5 = (i / m / m / m / m) % m + 'a';
        res &= is_true(strmap_set(strmap_p, (char[]){c5, c4, c3, c2, c1, '\0'}, (char[]){c1, c2, c3, c4, c5, '\0'}));
    }
    res &= is_equal(strmap_count(strmap_p), (size_t)lim);
    for (size_t i = 0; i < lim; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        char c4 = (i / m / m / m) % m + 'a';
        char c5 = (i / m / m / m / m) % m + 'a';
        res &= is_equal(strcmp(strmap_get(strmap_p, (char[]){c5, c4, c3, c2, c1, '\0'}), (char[]){c1, c2, c3, c4, c5, '\0'}), 0);
    }
    res &= is_true(strmap_deinit(&strmap_p));
    return res;
}

// bool for_each_test(void) {
// }

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
                                {del_and_exists_test, "delete and exists test"},
                                {missing_elements_test, "missing elements test"},
                                {overwrite_element_test, "overwrite element test"},
                                {empty_element_test, "empty element test"},
                                {million_elements_test, "million elements test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
