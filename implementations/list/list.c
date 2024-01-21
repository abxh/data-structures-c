#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

inline size_t roundup_powof2(size_t v) {
    // compute the next highest power of 2 of 32-bit v. portable and reasonably fast.
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
    return v + (v == 0);
}

inline size_t align(size_t n) {
    // Aligns the size by the machine word in bytes.
    //
    // sources:
    // http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/#memory-alignment
    // https://stackoverflow.com/a/29519151 (word size)
    typedef long word_t;
    return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

List* list_new(size_t data_size, size_t list_size) {
    if (data_size == 0 || list_size == 0) {
        return NULL;
    }
    size_t data_size_aligned = align(data_size);
    size_t list_size_actual = roundup_powof2(list_size);
    void* start_p = calloc(list_size_actual, data_size_aligned);
    if (start_p == NULL) {
        return NULL;
    }
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        free(start_p);
        return NULL;
    }
    list->data_size = data_size;
    list->data_size_aligned = data_size_aligned;
    list->capacity = list_size_actual;
    list->start_p = start_p;
    list->used_p = start_p - list->data_size_aligned;
    return list;
}

bool list_append(List* list, void* value_p, size_t size) {
    if (size > list->data_size) {
        return false;
    }
    if (list->length == list->capacity) {
        size_t list_size_new = list->capacity << 1;
        void* data_p_new = realloc(list->start_p, list_size_new);
        if (data_p_new == NULL) {
            return false;
        }
        list->start_p = data_p_new;
        list->capacity = list_size_new;
    }
    list->length += 1;
    list->used_p = (byte*)list->used_p + list->data_size_aligned;
    memcpy(list->used_p, value_p, size);
    return true;
}

void* list_pop(List* list) {
    if (list->length == 0) {
        return NULL;
    }
    void* value_p = NULL;
    memcpy(value_p, list->used_p, list->data_size);
    if (list->length == list->capacity >> 2) {
        size_t list_size_new = list->capacity >> 1;
        void* data_p_new = realloc(list->start_p, list_size_new);
        if (data_p_new == NULL) {
            return NULL;
        }
        list->start_p = data_p_new;
        list->capacity = list_size_new;
    }
    list->length -= 1;
    list->used_p = (byte*)list->used_p - list->data_size_aligned;
    return value_p;
}

void list_free(List* list) {
    free(list->start_p);
    free(list);
}
