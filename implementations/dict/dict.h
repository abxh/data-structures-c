#include <stdlib.h> // size_t, malloc, calloc, free, reallocarray, NULL
#include <stdint.h> // uint8_t, uint64_t
#include <stdbool.h> // bool, true, false
#include <string.h> // memcpy, memcmp
#include <assert.h> // assert, static_assert

typedef struct {
    void* key_p;
    void* value_p;
} DictEntry;

typedef struct {
    DictEntry* darr;
    size_t capacity;
    size_t used;
    size_t key_size;
    size_t value_size;
} Dict;

#define INITIAL_CAPACITY 128

static_assert(INITIAL_CAPACITY >= 8, "initial capacity must be larger than or equal to 8");
static_assert((INITIAL_CAPACITY & (INITIAL_CAPACITY - 1)) == 0, "initial capacity must be a power of 2");

/* Try create a new dict and returns it's pointer. Returns NULL if OOM. */
Dict* dict_new(size_t key_size, size_t value_size);

/* Check if a key exists. */
bool dict_exists(Dict* dict_p, void* key_p, size_t key_size);

/* Get the matching value of a key. Returns NULL if key is not found. */
void* dict_get(Dict* dict_p, void* key_p, size_t key_size);

/* Set a value of key to a value. Return if OOM. */
bool dict_set(Dict* dict_p, void* key_p, size_t key_size, void* value_p, size_t value_size);

/* Delete a key value pair. Return if key was found. */
bool dict_del(Dict* dict_p, void* key_p, size_t key_size);

/* Free the memory of the dict appropiately. */
void dict_free(Dict* dict_p);
