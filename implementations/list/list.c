#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

bool list_resize(List* list) {
    size_t list_capacity_new = 0;
    if (list->size == list->capacity) {
        list_capacity_new = list->capacity << 1;
    } else if (list->size == list->capacity >> 2) {
        list_capacity_new = list->capacity >> 1;
    } else {
        return true;
    }
    void* data_p_new = realloc(list->start_p, list_capacity_new);
    if (data_p_new == NULL) {
        return false;
    }
    list->start_p = data_p_new;
    list->capacity = list_capacity_new;
    return true;
}

size_t roundup_powof2(size_t v) {
    // compute the next highest power of 2 of 32-bit v.
    // portable and reasonably fast.
    //
    // exception:
    // when v == 0, then it returns 0.
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

size_t roundup_log2(size_t v) {
    // compute the  log base 2 of an 32-bit integer v with a lookup table.
    // portable and reasonably fast.
    //
    // source:
    // - http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup

    static const char LogTable256[256] = 
    {
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
        -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };

    unsigned r;     // r will be lg(v)
    register unsigned int t, tt; // temporaries

    if ((tt = v >> 16))
    {
      r = (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    }
    else 
    {
      r = (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
    }
    return r;
}

List* list_new(size_t data_size, size_t list_size) {
    if (data_size == 0 || list_size == 0) {
        return NULL;
    }
    size_t data_size_aligned = roundup_powof2(data_size);
    size_t data_size_aligned_log2 = roundup_log2(data_size_aligned);
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
    list->data_size_aligned_log2 = data_size_aligned_log2;
    list->capacity = list_size_actual;
    list->start_p = start_p;
    list->used_p = start_p;
    return list;
}

bool list_append(List* list, void* value_p, size_t size) {
    if (size > list->data_size) {
        return false;
    }
    if (!list_resize(list)) {
        return false;
    }
    memcpy(list->used_p, value_p, size);
    list->size += 1;
    list->used_p = (byte*)list->used_p + list->data_size_aligned;
    return true;
}

void* list_pop(List* list) {
    if (list->size == 0) {
        return NULL;
    }
    void* value_p = NULL;
    memcpy(list->used_p, value_p, list->data_size);
    if (!list_resize(list)) {
        free(value_p);
        return false;
    }
    
    list->size -= 1;
    list->used_p = (byte*)list->used_p - list->data_size_aligned;
    return value_p;
}

void list_free(List* list) {
    free(list->start_p);
    free(list);
}
