#include "assert.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#include "eval_and_log.h"
#define el(expr) eval_and_log(expr)

#include "../src/strmap/strmap.h"

bool one_element_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(strmap_count(strmap_p) == 0);
    res &= el(strmap_set(strmap_p, "AA", "11"));
    res &= el(strmap_count(strmap_p) == 1);
    res &= el(strcmp(strmap_get(strmap_p, "AA"), "11") == 0);
    res &= el(strmap_deinit(&strmap_p));
    return res;
}

bool four_elements_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(strmap_set(strmap_p, "AAA", "1234"));
    res &= el(strmap_set(strmap_p, "BBB", "5678"));
    res &= el(strmap_set(strmap_p, "CCC", "9012"));
    res &= el(strmap_set(strmap_p, "DDD", "3456"));

    res &= el(strcmp(strmap_get(strmap_p, "AAA"), "1234") == 0);
    res &= el(strcmp(strmap_get(strmap_p, "BBB"), "5678") == 0);
    res &= el(strcmp(strmap_get(strmap_p, "CCC"), "9012") == 0);
    res &= el(strcmp(strmap_get(strmap_p, "DDD"), "3456") == 0);

    res &= el(strmap_deinit(&strmap_p));

    return res;
}

bool del_and_exists_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(strmap_set(strmap_p, "aaa", "1234"));
    res &= el(strmap_set(strmap_p, "aab", "5678"));
    res &= el(strmap_set(strmap_p, "aac", "9012"));
    res &= el(strmap_set(strmap_p, "aad", "3456"));

    res &= el(strmap_count(strmap_p) == 4);
    res &= strmap_del(strmap_p, "aab");
    res &= el(strmap_count(strmap_p) == 3);

    res &= el(!strmap_del(strmap_p, "aae"));

    res &= el(strmap_exists(strmap_p, "aaa") == true);
    res &= el(strmap_exists(strmap_p, "aab") == false);
    res &= el(strmap_exists(strmap_p, "aac") == true);
    res &= el(strmap_exists(strmap_p, "aad") == true);

    res &= el(strmap_deinit(&strmap_p));

    return res;
}

bool missing_elements_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(strmap_exists(strmap_p, "a") == false);
    res &= el(strmap_exists(strmap_p, "b") == false);
    res &= el(strmap_exists(strmap_p, "c") == false);
    res &= el(strmap_exists(strmap_p, "d") == false);

    res &= el(strmap_set(strmap_p, "a", "1234"));
    res &= el(strmap_set(strmap_p, "d", "3456"));

    res &= el(strmap_exists(strmap_p, "a") == true);
    res &= el(strmap_exists(strmap_p, "b") == false);
    res &= el(strmap_exists(strmap_p, "c") == false);
    res &= el(strmap_exists(strmap_p, "d") == true);

    res &= el(strmap_deinit(&strmap_p));

    return res;
}

bool overwrite_element_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(strmap_count(strmap_p) == 0);

    res &= el(strmap_set(strmap_p, "A", "B"));
    res &= el(strmap_count(strmap_p) == 1);

    res &= el(strmap_set(strmap_p, "A", "C"));
    res &= el(strmap_count(strmap_p) == 1);

    res &= el(strmap_set(strmap_p, "A", "C"));
    res &= el(strmap_count(strmap_p) == 1);

    res &= el(strcmp(strmap_get(strmap_p, "A"), "C") == 0);

    res &= el(strmap_deinit(&strmap_p));

    return res;
}

bool empty_element_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(strmap_set(strmap_p, "A", ""));
    res &= el(strcmp(strmap_get(strmap_p, "A"), "") == 0);
    res &= el(strmap_set(strmap_p, "", "B"));
    res &= el(strcmp(strmap_get(strmap_p, ""), "B") == 0);
    res &= el(strmap_set(strmap_p, "", ""));
    res &= el(strcmp(strmap_get(strmap_p, ""), "") == 0);

    res &= el(strmap_deinit(&strmap_p));

    return res;
}

#define lim 1000000
#define m ('z' - 'a' + 1)

bool million_elements_test(void) {
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    res &= strmap_count(strmap_p) == 0;
    for (size_t i = 0; i < lim; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        char c4 = (i / m / m / m) % m + 'a';
        char c5 = (i / m / m / m / m) % m + 'a';
        res &= el(strmap_set(strmap_p, (char[]){c5, c4, c3, c2, c1, '\0'}, (char[]){c1, c2, c3, c4, c5, '\0'}));
    }
    res &= strmap_count(strmap_p) == lim;
    for (size_t i = 0; i < lim; i++) {
        char c1 = i % m + 'a';
        char c2 = (i / m) % m + 'a';
        char c3 = (i / m / m) % m + 'a';
        char c4 = (i / m / m / m) % m + 'a';
        char c5 = (i / m / m / m / m) % m + 'a';
        res &= el(strcmp(strmap_get(strmap_p, (char[]){c5, c4, c3, c2, c1, '\0'}), (char[]){c1, c2, c3, c4, c5, '\0'}) == 0);
    }
    res &= el(strmap_deinit(&strmap_p));
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    func_plus bool_f_arr[] = {{one_element_test, "one element test"},
                              {four_elements_test, "four elements test"},
                              {del_and_exists_test, "delete and exists test"},
                              {missing_elements_test, "missing elements test"},
                              {overwrite_element_test, "overwrite element test"},
                              {empty_element_test, "empty element test"},
                              {million_elements_test, "million elements test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
