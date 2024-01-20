#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t used;
    size_t list_size;
    size_t data_size;
    void* used_p;
    void* data_p;
} List;

List* list_new(size_t data_size, size_t list_size);

static inline void* list_get(const List* list, size_t index) {
    if (index >= list->used) {
        return NULL;
    }
    return (char*)list->data_p + index * list->data_size;
}

static inline bool list_set(const List* list, size_t index, void* value_p) {
    if (index >= list->used) {
        return false;
    }
    memcpy((char*)list->data_p + index * list->data_size, value_p, list->data_size);
    return true;
}

bool list_append(List* list, void* value_p);

void* list_pop(List* list);

void list_free(List* list);
