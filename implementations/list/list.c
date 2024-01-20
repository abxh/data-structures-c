#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

size_t roundup_powof2(size_t v) {
    // compute the next highest power of 2 of 32-bit v. portable and reasonably fast.
    //
    // source:
    // - http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2

    _Static_assert(sizeof(size_t) == 8, "Expected size_t to be 8 bytes");

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v + (v == 0);
}

List* list_new(size_t data_size, size_t list_size) {
    if (data_size == 0 || list_size == 0) {
        return NULL;
    }
    size_t data_size_new = data_size;
    size_t list_size_new = roundup_powof2(list_size);
    void* data_p = calloc(list_size_new, data_size_new);
    if (data_p == NULL) {
        return NULL;
    }
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        free(data_p);
        return NULL;
    }
    list->data_p = data_p;
    list->data_size = data_size_new;
    list->list_size = list_size_new;
    list->used_p = data_p;
    return list;
}

bool list_append(List* list, void* value_p_new) {
    if (list->used == list->list_size) {
        size_t list_size_new = list->list_size << 1;
        void* data_p_new = realloc(list->data_p, list_size_new);
        if (data_p_new == NULL) {
            return false;
        }
        list->data_p = data_p_new;
        list->list_size = list_size_new;
    }
    list->used += 1;
    list->used_p += list->data_size;
    memcpy(list->used_p, value_p_new, list->data_size);
    return true;
}

void* list_pop(List* list) {
    if (list->used == 0) {
        return NULL;
    }
    void* value_p = NULL;
    memcpy(value_p, list->used_p, list->data_size);
    if (list->used == list->list_size >> 2) {
        size_t list_size_new = list->list_size >> 1;
        void* data_p_new = realloc(list->data_p, list_size_new);
        if (data_p_new == NULL) {
            return NULL;
        }
        list->data_p = data_p_new;
        list->list_size = list_size_new;
    }
    list->used -= 1;
    list->used_p -= list->data_size;
    return value_p;
}

void list_free(List* list) {
    free(list->data_p);
    free(list);
}
