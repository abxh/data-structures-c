#include <stdbool.h> // bool, true, false
#include <stdint.h> // SIZE_MAX
#include <stdio.h>  // printf
#include <stdlib.h> // free

#include "dict.h" // dict_*

// note:
// run using valgrind as well

bool size_t_is_equal(unsigned char* x, unsigned char* y) {
    return (size_t) x == (size_t) y;
}

bool million_entry_test(void) {
    Dict* dict_p = dict_new(32, sizeof(size_t), sizeof(size_t), fnv_hash64, size_t_is_equal);
    if (dict_p == NULL) {
        return false;
    }
    bool res = true;
     for (size_t i = 0; i < 32; i++) {
        size_t value = SIZE_MAX - i;
        dict_set(dict_p, (unsigned char*)&i,(unsigned char*) &value);
    }
    for (size_t i = 0; i < 32; i++) {
        size_t expected_value = SIZE_MAX - i;
        unsigned char* value_p = dict_get(dict_p,(unsigned char*) &i);
        res &= value_p != NULL ? (expected_value == *(size_t*)value_p) : false;
    }
    dict_free(&dict_p);
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

int main(void) {
    func_plus bool_f_arr[] = {
        {million_entry_test, "million entry test"},
    };
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(*bool_f_arr); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
