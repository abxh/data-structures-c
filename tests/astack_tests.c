#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf

#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src/astack/astack.h"

bool empty_test(void) {
    astack_int* s;
    if (!astack_int_init_with_capacity(&s, 1)) {
        return false;
    }
    bool res = true;

    res &= astack_int_capacity(s) == 1;

    res &= astack_int_is_empty(s);
    res &= astack_int_count(s) == 0;
    res &= astack_int_deinit(&s);

    return res;
}

bool one_element_test(void) {
    astack_int* s;
    if (!astack_int_init_with_capacity(&s, 1)) {
        return false;
    }
    bool res = true;

    res &= astack_int_capacity(s) == 1;

    int value = 5;

    res &= astack_int_count(s) == 0;
    astack_int_push(s, value);
    res &= astack_int_count(s) == 1;

    res &= value == astack_int_peek(s);
    res &= !astack_int_is_empty(s);

    res &= value == astack_int_pop(s);
    res &= astack_int_count(s) == 0;

    res &= astack_int_is_empty(s);
    res &= !astack_int_is_full(s);

    res &= astack_int_deinit(&s);

    return res;
}

bool multiple_elements_test(void) {
    astack_int* s;
    if (!astack_int_init_with_capacity(&s, 10)) {
        return false;
    }
    bool res = true;

    res &= astack_int_capacity(s) == 10;

    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    res &= astack_int_count(s) == 0;
    astack_int_push_many(s, arr, 10);
    res &= astack_int_count(s) == 10;

    for (int i = 10; i >= 1; i--) {
        res &= i == astack_int_pop(s);
    }
    res &= astack_int_count(s) == 0;

    res &= astack_int_deinit(&s);
    return res;
}

bool million_elements_test(void) {
    astack_int* s;
    if (!astack_int_init_with_capacity(&s, 1000000)) {
        return false;
    }
    bool res = true;

    res &= astack_int_capacity(s) == 1000000;

    for (int i = 1; i <= 1000000; i++) {
        astack_int_push(s, i);
        res &= astack_int_count(s) == (size_t)i;
    }

    for (int i = 1000000; i >= 1; i--) {
        res &= astack_int_count(s) == (size_t)i;
        res &= i == astack_int_pop(s);
    }

    res &= astack_int_deinit(&s);
    return res;
}

bool for_each_test(void) {
    astack_int* s;
    if (!astack_int_init_with_capacity(&s, 50)) {
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        astack_int_push(s, i);
    }
    int value = 51;
    int var;
    size_t i;
    astack_for_each(s, var, i) {
        res &= value == var;
        value++;
    }
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
    func_plus bool_f_arr[] = {{empty_test, "empty test"},
                              {one_element_test, "one element test"},
                              {multiple_elements_test, "multiple elements test"},
                              {million_elements_test, "million elements test"},
                              {for_each_test, "for each test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
