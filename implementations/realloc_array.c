#include <stdlib.h>
#include <stdbool.h>

/* reallocate array such that it dynamically grows and shrinks as needed. */
bool realloc_array(void* arr_p, size_t* capacity_p, size_t size) {
    size_t capacity_new = 0;
    if (size == *capacity_p) {
        capacity_new = *capacity_p << 1;
    } else if (size == *capacity_p >> 2) {
        capacity_new = *capacity_p >> 1;
    } else {
        return true;
    }
    void* data_p_new = realloc(arr_p, capacity_new);
    if (data_p_new == NULL) {
        return false;
    }
    *capacity_p = capacity_new;
    return true;
}
