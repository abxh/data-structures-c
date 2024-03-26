#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf

#include "eval_bool_and_log.h"
#define el(expr) eval_bool_and_log(expr)

#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src/astack/astack.h"

bool empty_test(void) {
    astack_int* s;
    if (!astack_int_init(&s, 1)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(astack_int_capacity(s) == 1);

    res &= el(astack_int_is_empty(s));
    res &= el(astack_int_count(s) == 0);
    res &= el(astack_int_deinit(&s));

    return res;
}

bool one_element_test(void) {
    astack_int* s;
    if (!astack_int_init(&s, 1)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(astack_int_capacity(s) == 1);

    int value = 5;

    res &= el(astack_int_count(s) == 0);
    astack_int_push(s, value);
    res &= el(astack_int_count(s) == 1);

    res &= el(value == astack_int_peek(s));
    res &= el(!astack_int_is_empty(s));

    res &= el(value == astack_int_pop(s));
    res &= el(astack_int_count(s) == 0);

    res &= el(astack_int_is_empty(s));
    res &= el(!astack_int_is_full(s));

    res &= el(astack_int_deinit(&s));

    return res;
}

bool million_elements_test(void) {
    astack_int* s;
    if (!astack_int_init(&s, 1000000)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(astack_int_capacity(s) == 1000000);

    for (int i = 1; i <= 1000000; i++) {
        astack_int_push(s, i);
        res &= el(astack_int_count(s) == (size_t)i);
    }

    for (int i = 1000000; i >= 1; i--) {
        res &= el(astack_int_count(s) == (size_t)i);
        res &= el(i == astack_int_pop(s));
    }

    res &= el(astack_int_deinit(&s));
    return res;
}

bool for_each_test(void) {
    astack_int* s;
    if (!astack_int_init(&s, 50)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
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
        res &= el(value == var);
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
                              {million_elements_test, "million elements test"},
                              {for_each_test, "for each test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
