
#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf
#include <stdlib.h>  // free

#include "queue.h" // queue_*

// note:
// test using valgrind as well.

bool empty_test(void) {
    Queue* queue_p = queue_new(sizeof(int));
    bool res = true; // queue_isempty(queue_p);
    queue_free(queue_p);
    return res;
}

bool one_element_test(void) {
    Queue* queue_p = queue_new(sizeof(int));
    bool res = true;
    int value = 5;
    res &= queue_enqueue(queue_p, &value, sizeof(int));
    res &= value == *(int*)queue_peek(queue_p);
    void* value_p_rtr = queue_dequeue(queue_p, sizeof(int));
    res &= value == *(int*)value_p_rtr;
    free(value_p_rtr);
    res &= queue_isempty(queue_p);
    queue_free(queue_p);
    return res;
}

bool million_element_test(void) {
    Queue* queue_p = queue_new(sizeof(int));
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        res &= queue_enqueue(queue_p, &i, sizeof(int));
        if (!res) {
            queue_free(queue_p);
            return false;
        }
    }
    for (int i = 1; i <= 1000000; i++) {
        void* value_p_rtr = queue_dequeue(queue_p, sizeof(int));
        res &= i == *(int*)value_p_rtr;
        free(value_p_rtr);
    }
    queue_free(queue_p);
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
        {empty_test, "empty test"}, {one_element_test, "one element test"}, {million_element_test, "million element test"}};
    for (int i = 0; i < SIZEOF(bool_f_arr); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
