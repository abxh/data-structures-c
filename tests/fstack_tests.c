#include <stdbool.h>
#include <stdio.h>

#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src/fstack.h"

bool empty_test(void) {
    FStack* s;
    if (!fstack_init_int(&s, 1)) {
        return false;
    }
    bool res = true;

    res &= fstack_isempty(s);
    res &= fstack_deinit(&s);

    return res;
}

bool one_element_test(void) {
    FStack* s;
    if (!fstack_init_int(&s, 1)) {
        return false;
    }
    bool res = true;
    int value = 5;
    fstack_push_int(s, value);

    res &= value == fstack_peek_int(s);
    res &= !fstack_isempty(s);

    res &= value == fstack_pop_int(s);
    res &= fstack_isempty(s);
    res &= !fstack_isfull(s);

    res &= fstack_deinit(&s);

    return res;
}

bool million_elements_test(void) {
    FStack* s;
    if (!fstack_init_int(&s, 1000000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        fstack_push_int(s, i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == fstack_pop_int(s);
    }
    res &= fstack_deinit(&s);
    return res;
}

bool grow_test(void) {
    FStack* s;
    if (!fstack_init_int(&s, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 500000; i++) {
        fstack_push_int(s, i);
    }

    res &= fstack_isfull(s);
    if (!fstack_resize(s, s->used << 1)) {
        fstack_deinit(&s);
        return false;
    }
    res &= !fstack_isfull(s);

    for (int i = 500001; i <= 1000000; i++) {
        fstack_push_int(s, i);
    }
    for (int i = 1000000; i >= 1; i--) {
        res &= i == fstack_pop_int(s);
    }
    res &= fstack_deinit(&s);
    return res;
}

bool shrink_test(void) {
    FStack* s;
    if (!fstack_init_int(&s, 500000)) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 250000; i++) {
        fstack_push_int(s, i);
    }

    res &= !fstack_isfull(s);
    if (!fstack_resize(s, s->used)) {
        fstack_deinit(&s);
        return false;
    }
    res &= fstack_isfull(s);

    for (int i = 250000; i >= 1; i--) {
        res &= i == fstack_pop_int(s);
    }
    res &= fstack_deinit(&s);
    return res;
}

bool foreach_test(void) {
    FStack* s;
    if (!fstack_init_int(&s, 50)) {
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        fstack_push_int(s, i);
    }
    int x = 100;
    int value;
    size_t i;
    stack_foreach(s, i, value) {
        res &= x == value;
        x--;
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
        {grow_test, "grow test"},   {shrink_test, "shrink test"},           {foreach_test, "foreach test"}};
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf("[%s] %s\n", bool_f_arr[i].func() ? "true" : "false", bool_f_arr[i].desc);
    }
    return 0;
}
