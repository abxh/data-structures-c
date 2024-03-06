#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf

#define TYPE int
#define NAME int
#include "../src/stack.h"

bool empty_test(void) {
    Stack* s;
    if (!stack_init(&s, 1, sizeof(int))) {
        return false;
    }
    bool res = true;
    res &= stack_isempty(s);
    res &= stack_free(&s);
    return res;
}

bool one_element_test(void) {
    Stack* s;
    if (!stack_init(&s, 1, sizeof(int))) {
        return false;
    }
    bool res = true;
    int value = 5;
    stack_push_int(s, value);
    res &= value == stack_peek_int(s);
    res &= !stack_isempty(s);
    res &= value == stack_pop_int(s);
    res &= stack_isempty(s);
    res &= !stack_isfull(s);
    res &= stack_free(&s);
    return res;
}

bool million_elements_test(void) {
    Stack* s;
    if (!stack_init(&s, 1000000, sizeof(int))) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        stack_push_int(s, i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == stack_pop_int(s);
    }
    res &= stack_free(&s);
    return res;
}

bool grow_test(void) {
    Stack* s;
    if (!stack_init(&s, 500000, sizeof(int))) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        stack_push_int(s, i);
    }
    res &= stack_isfull(s);
    res &= stack_resize(s, s->used << 1);
    res &= !stack_isfull(s);
    for (int i = 500001; i <= 1000000; i++) {
        stack_push_int(s, i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == stack_pop_int(s);
    }
    res &= stack_free(&s);
    return res;
}

bool shrink_test(void) {
    Stack* s;
    if (!stack_init(&s, 500000, sizeof(int))) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        stack_push_int(s, i);
    }
    res &= !stack_isfull(s);
    res &= stack_resize(s, s->used);
    res &= stack_isfull(s);
    for (int i = 250000; i >= 1; i--) {
        res &= i == stack_pop_int(s);
    }
    res &= stack_free(&s);
    return res;
}

bool foreach_test(void) {
    Stack* s;
    if (!stack_init(&s, 50, sizeof(int))) {
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        stack_push_int(s, i);
    }
    int value = 51;
    int var;
    stack_foreach(s, var) {
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

int main(void) {
    func_plus bool_f_arr[] = {
        {empty_test, "empty test"}, {one_element_test, "one element test"}, {million_elements_test, "million elements test"},
        {grow_test, "grow test"},   {shrink_test, "shrink test"}, {foreach_test, "foreach test"}};
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
