#include <stdbool.h>
#include <stdio.h>

#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src/astack.h"

bool empty_test(void) {
    AStack* s;
    if (!astack_init_int(&s, 1)) {
        return false;
    }
    bool res = true;

    res &= astack_isempty(s);
    res &= astack_deinit(&s);

    return res;
}

bool one_element_test(void) {
    AStack* s;
    if (!astack_init_int(&s, 1)) {
        return false;
    }
    bool res = true;
    int value = 5;

    res &= astack_capacity(s) == 1;

    astack_push_int(s, value);
    res &= value == astack_peek_int(s);
    res &= !astack_isempty(s);
    res &= astack_isfull(s);
    res &= astack_length(s) == 1;

    res &= astack_capacity(s) == 1;

    res &= value == astack_pop_int(s);
    res &= astack_isempty(s);
    res &= !astack_isfull(s);
    res &= astack_length(s) == 0;

    res &= astack_capacity(s) == 1;

    res &= astack_deinit(&s);

    return res;
}

bool million_elements_test(void) {
    AStack* s;
    if (!astack_init_int(&s, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        astack_push_int(s, i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == astack_pop_int(s);
    }
    res &= astack_deinit(&s);
    return res;
}

bool foreach_test(void) {
    AStack* s;
    if (!astack_init_int(&s, 50)) {
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        astack_push_int(s, i);
    }
    int x = 100;
    int value;
    size_t i;
    astack_foreach(s, i, value) {
        res &= x == value;
        x--;
    }
    res &= astack_deinit(&s);
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
                              {foreach_test, "foreach test"}};

    printf(__FILE_NAME__ " tests:\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
