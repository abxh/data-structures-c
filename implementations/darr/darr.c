#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "darr.h"

_Static_assert(sizeof(size_t) == 8, "Expected size_t to be 8 bytes");

size_t roundup_powof2(size_t v) {
    // compute the next highest power of 2 of 32-bit v. portable and reasonably fast.
    //
    // exception:
    // - does not work for the value 0, for which it returns 0.
    //
    // source:
    // - http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

Darr* darr_new(size_t data_size, size_t initial_array_size) {
    size_t data_size_new = data_size + (data_size == 0);
    size_t array_size_new = roundup_powof2(initial_array_size);
    void* data_p = calloc(array_size_new, data_size_new);
    if (data_p == NULL) {
        return NULL;
    }
    Darr* darr_container = (Darr*)malloc(sizeof(Darr));
    if (darr_container == NULL) {
        free(data_p);
        return NULL;
    }
    darr_container->data_p = data_p;
    darr_container->data_size = data_size_new;
    darr_container->array_size = array_size_new;
    darr_container->used_p = data_p;
    return darr_container;
}

bool darr_insert(Darr* darr, void* value_p_new) {
    if (darr->used == darr->array_size) {
        darr->array_size *= 2;
        void* data_p_new = realloc(darr->data_p, darr->array_size);
        if (data_p_new == NULL) {
            // free memory and terminate program?. TODO
            return false;
        }
        darr->data_p = data_p_new;
    }
    darr->used += 1;
    darr->used_p += darr->data_size;
    memcpy(darr->used_p, value_p_new, darr->data_size);
    return true;
}

void* darr_remove(Darr* darr) {
    if (darr->used == 0) {
        return NULL;
    }
    void* value_p = NULL;
    memcpy(value_p, darr->used_p, darr->data_size);
    if (darr->used == darr->array_size / 4) {
        darr->array_size /= 2;

        void* data_p_new = realloc(darr->data_p, darr->array_size);
        if (data_p_new == NULL) {
            // free memory and terminate program?. TODO
            return NULL;
        }
        darr->data_p = data_p_new;
    }
    darr->used -= 1;
    darr->used_p -= darr->data_size;
    return value_p;
}
