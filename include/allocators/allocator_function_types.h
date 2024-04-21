#include <stdlib.h> // size_t

typedef void* (*allocate_f)(void*, size_t, size_t); // allocator_struct_p, alignment, size

typedef void* (*reallocate_f)(void*, void*, size_t, size_t, size_t); // allocator_struct_p, ptr, alignment, old_size, new_size

typedef void (*deallocate_f)(void*, void*); // allocator_struct_p, ptr
