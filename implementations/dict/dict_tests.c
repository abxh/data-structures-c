#include <stdbool.h> // bool, true, false
#include <stdint.h> // SIZE_MAX
#include <stdio.h>  // printf
#include <stdlib.h> // free

#include "dict.h" // dict_*

// note:
// run using valgrind as well

bool one_entry_test(void) {
    int key = 42;
    int value = 420;
    Dict* dict_p = dict_new(1, sizeof(key), sizeof(value));
    if (dict_p == NULL) {
        return false;
    }
    bool res = true;

    res &= !dict_exists(dict_p, &key, sizeof(key));
    res &= NULL == dict_get(dict_p, &key, sizeof(key));
    res &= !dict_del(dict_p, &key, sizeof(key));

    res &= dict_set(dict_p, &key, sizeof(key), &value, sizeof(value));
    res &= dict_exists(dict_p, &key, sizeof(key));
    void* value_p = dict_get(dict_p, &key, sizeof(key));
    res &= value_p != NULL ? (value == *(int*)value_p) : false;

    res &= dict_del(dict_p, &key, sizeof(key));
    res &= !dict_exists(dict_p, &key, sizeof(key));
    res &= NULL == dict_get(dict_p, &key, sizeof(key));
    res &= !dict_del(dict_p, &key, sizeof(key));

    dict_free(dict_p);
    return res;
}

bool two_entry_test(void) {
    int key1 = 1;
    int value1 = 2;
    int key2 = 3;
    int value2 = 4;
    Dict* dict_p = dict_new(1, sizeof(key1), sizeof(value1));
    if (dict_p == NULL) {
        return false;
    }
    bool res = true;

    res &= dict_set(dict_p, &key1, sizeof(key1), &value1, sizeof(value1));
    res &= dict_exists(dict_p, &key1, sizeof(key1));
    void* value_p = dict_get(dict_p, &key1, sizeof(key1));
    res &= value_p != NULL ? (value1 == *(int*)value_p) : false;

    res &= dict_set(dict_p, &key2, sizeof(key2), &value2, sizeof(value2));
    res &= dict_exists(dict_p, &key2, sizeof(key2));
    value_p = dict_get(dict_p, &key2, sizeof(key2));
    res &= value_p != NULL ? (value2 == *(int*)value_p) : false;

    res &= dict_del(dict_p, &key1, sizeof(key1));
    res &= dict_del(dict_p, &key2, sizeof(key2));

    res &= !dict_exists(dict_p, &key1, sizeof(key1));
    res &= NULL == dict_get(dict_p, &key1, sizeof(key1));
    res &= !dict_del(dict_p, &key1, sizeof(key1));

    res &= !dict_exists(dict_p, &key2, sizeof(key2));
    res &= NULL == dict_get(dict_p, &key2, sizeof(key2));
    res &= !dict_del(dict_p, &key2, sizeof(key2));

    res &= dict_set(dict_p, &key1, sizeof(key1), &value1, sizeof(value1));
    res &= dict_exists(dict_p, &key1, sizeof(key1));
    value_p = dict_get(dict_p, &key1, sizeof(key1));
    res &= value_p != NULL ? (value1 == *(int*)value_p) : false;

    res &= dict_set(dict_p, &key2, sizeof(key2), &value2, sizeof(value2));
    res &= dict_exists(dict_p, &key2, sizeof(key2));
    value_p = dict_get(dict_p, &key2, sizeof(key2));
    res &= value_p != NULL ? (value2 == *(int*)value_p) : false;

    dict_free(dict_p);
    return res;
}

bool million_entry_test(void) {
    Dict* dict_p = dict_new(1, sizeof(size_t), sizeof(size_t));
    if (dict_p == NULL) {
        return false;
    }
    bool res = true;
    for (size_t i = 0; i < 32; i++) {
        size_t value = SIZE_MAX - i;
        res &= dict_set(dict_p, &i, sizeof(i), &value, sizeof(value));
    }
    for (size_t i = 0; i < 32; i++) {
        size_t expected_value = SIZE_MAX - i;
        void* value_p = dict_get(dict_p, &i, sizeof(i));
        res &= value_p != NULL ? (expected_value == *(size_t*)value_p) : false;
    }
    dict_free(dict_p);
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

int main(void) {
    func_plus bool_f_arr[] = {
        {one_entry_test, "one entry test"},
        {two_entry_test, "two entry test"},
        {million_entry_test, "million entry test"},
    };
    for (size_t i = 0; i < SIZEOF(bool_f_arr); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
