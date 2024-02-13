#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf

#include "stack.h" // stack_*

// note:
// test using valgrind as well.

bool empty_test(void) {
    Stack* stack_p = stack_new(1, sizeof(int));
    bool res = true; // stack_isempty(stack_p);
    stack_free(&stack_p);
    return res;
}

bool one_element_test(void) {
    Stack* stack_p = stack_new(1, sizeof(int));
    bool res = true;
    int value = 5;
    stack_push(stack_p, (unsigned char*)&value);
    res &= value == *(int*)stack_peek(stack_p);
    res &= !stack_isempty(stack_p);
    res &= value == *(int*)stack_pop(stack_p);
    res &= stack_isempty(stack_p);
    res &= !stack_isfull(stack_p);
    stack_free(&stack_p);
    return res;
}

bool million_elements_test(void) {
    Stack* stack_p = stack_new(1000000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        stack_push(stack_p, (unsigned char*)&i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == *(int*)stack_pop(stack_p);
    }
    stack_free(&stack_p);
    return res;
}

bool grow_test(void) {
    Stack* stack_p = stack_new(500000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        stack_push(stack_p, (unsigned char*)&i);
    }
    res &= stack_isfull(stack_p);
    res &= stack_resize(stack_p, stack_p->used << 1);
    res &= !stack_isfull(stack_p);
    for (int i = 500001; i <= 1000000; i++) {
        stack_push(stack_p, (unsigned char*)&i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == *(int*)stack_pop(stack_p);
    }
    stack_free(&stack_p);
    return res;
}

bool shrink_test(void) {
    Stack* stack_p = stack_new(500000, sizeof(int));
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        stack_push(stack_p, (unsigned char*)&i);
    }
    res &= !stack_isfull(stack_p);
    res &= stack_resize(stack_p, stack_p->used);
    res &= stack_isfull(stack_p);
    for (int i = 250000; i >= 1; i--) {
        res &= i == *(int*)stack_pop(stack_p);
    }
    stack_free(&stack_p);
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

int main(void) {
    func_plus bool_f_arr[] = {{empty_test, "empty test"},
                              {one_element_test, "one element test"},
                              {million_elements_test, "million elements test"},
                              {grow_test, "grow test"},
                              {shrink_test, "shrink test"}};
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
