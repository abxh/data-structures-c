#include "stack.h"
#include <stdbool.h>
#include <stdio.h>

// note:
// test using sanitizers as well.

bool empty_test(void) {
    Stack* stack_p = stack_new(sizeof(int));
    bool res = true; // stack_isempty(stack_p);
    stack_free(stack_p);
    return res;
}

bool one_node_test(void) {
    Stack* stack_p = stack_new(sizeof(int));
    bool res = true;
    int value = 5;
    stack_push(stack_p, &value, sizeof(int));
    res &= value == *(int*) stack_peek(stack_p);
    void* value_p_rtr = stack_pop(stack_p, sizeof(int));
    res &= value == *(int*) value_p_rtr;
    free(value_p_rtr);
    res &= stack_isempty(stack_p);
    stack_free(stack_p);
    return res;
}

bool million_nodes_test(void) {
    Stack* stack_p = stack_new(sizeof(int));
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        res &= stack_push(stack_p, &i, sizeof(int));
        if (!res) {
            stack_free(stack_p);
            return false;
        }
    }
    for (int i = 1000000; i >= 1; i--) {
        void* value_p_rtr = stack_pop(stack_p, sizeof(int));
        res &= i == *(int*) value_p_rtr;
        free(value_p_rtr);
    }
    stack_free(stack_p);
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

int main(void) {
    func_plus bool_f_arr[3] = {
        {empty_test, "empty test"},
        {one_node_test, "one node test"},
        {million_nodes_test, "billion nodes test"}
    };
    for (int i = 0; i < 3; i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
