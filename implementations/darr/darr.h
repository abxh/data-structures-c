#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    size_t used;
    size_t array_size;
    size_t data_size;
    void* used_p;
    void* data_p;
} Darr;

Darr* darr_new(size_t size, size_t nmemb);

static inline void* darr_get(const Darr* darr, size_t index) {
    return index < darr->used ? (char*)darr->data_p + index * darr->data_size : NULL;
}

bool darr_insert(Darr* array, void* value_p);

void* darr_remove(Darr* array);
