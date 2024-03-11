#include <stdbool.h>
#include <stdio.h>

#include "../src/stack/stack.h"

#define VALUE_LABEL int
#define VALUE_TYPE int
#include "../src/stack/stack_helpers.h"

bool empty_test(void) {
    Stack* s;
    if (!stack_init(&s, sizeof(int))) {
        return false;
    }
    bool res = true;

    res &= stack_count(s) == 0;
    res &= stack_isempty(s);
    res &= stack_deinit(&s);

    return res;
}

bool one_element_test(void) {
    Stack* s;
    if (!stack_init(&s, sizeof(int))) {
        return false;
    }
    bool res = true;
    int value = 5;

    res &= stack_push_int(s, value);
    res &= value == stack_peek_int(s);
    res &= !stack_isempty(s);
    res &= stack_count(s) == 1;

    res &= value == stack_pop_int(s);
    res &= stack_isempty(s);
    res &= stack_count(s) == 0;

    res &= stack_deinit(&s);

    return res;
}

bool million_elements_test(void) {
    Stack* s;
    if (!stack_init(&s, sizeof(int))) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        stack_push_int(s, i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == stack_pop_int(s);
    }
    res &= stack_deinit(&s);
    return res;
}

bool foreach_test(void) {
    Stack* s;
    if (!stack_init(&s, sizeof(int))) {
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        stack_push_int(s, i);
    }
    int x = 100;
    StackNode* node_p;
    int value;
    stack_foreach(s, node_p, value) {
        res &= x == value;
        x--;
    }
    res &= stack_deinit(&s);
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
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}
