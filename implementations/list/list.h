#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(size_t) == 8, "Code is assumed to run on a 64-bit machine.");

typedef struct {
    size_t length;
    size_t capacity;
    size_t data_size;
    size_t data_size_aligned;
    void* start_p;
    void* used_p;
} List;

typedef uint8_t byte;

/* Create a new list and returns it's pointer. Can return NULL and it should be
 * checked manually. */
List* list_new(size_t data_size, size_t list_size);

/* Get the length of the list. */
static inline size_t list_length(const List* list) {
    return list->length;
}

/* Get the pointer of a value at an index. */
static inline void* list_get(const List* list, size_t index) {
    if (index >= list->length) {
        return NULL;
    }
    return (byte*)list->start_p + index * list->data_size_aligned;
}

/* Set the value at an index by providing the value pointer and a size that does
 *  exceed data_size. */
static inline bool list_set(const List* list, size_t index, void* value_p, size_t size) {
    if (index >= list->length || size > list->data_size) {
        return false;
    }
    memcpy((byte*)list->start_p + index * list->data_size_aligned, value_p, size);
    return true;
}

/* Append a value at an index at the end of the list by providing the value
 * pointer and a size that does exceed data_size. */
bool list_append(List* list, void* value_p, size_t size);

/* Pop the value at the end of the list and return it's pointer. */
void* list_pop(List* list);

/* Free the memory of the list appropiately. */
void list_free(List* list);
