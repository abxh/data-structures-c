/**
 * @file std_allocator.h
 * @brief wrapper of std functions for conforming to the defined interface of allocator operations.
 */

#pragma once

#include "allocator_ops.h" // allocator_ops_type
#include <stdlib.h>        // size_t, malloc, realloc, free, size_t

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * @brief `malloc` wrapper to allocate memory.
 * @note Can pass NULL as context\_ptr.
 *
 * @param[in] context\_ptr (unused)
 * @param[in] alignment (unused)
 * @param[in] size Amount of memory to allocate in bytes.
 * @return The stack pointer.
 * @retval NULL
 * - Implementation-defined, but may do so if size is 0.
 * - If no memory space is available.
 */
static inline void* std_allocate(void* context_ptr, size_t alignment, size_t size) {
    return malloc(size);
}

/**
 * @brief `realloc` wrapper to allocate memory.
 * @note Can pass NULL as context\_ptr.
 *
 * @warning Passing memory_ptr not allocated by malloc, calloc or free is UB.
 *
 * @param[in] context\_ptr (unused)
 * @param[in] mem\_ptr Pointer to the memory buffer. If NULL, then malloc(size) is called.
 * @param[in] alignment (unused)
 * @param[in] old_size (unused)
 * @param[in] new_size Amount of memory to allocate in bytes.
 * @return A new pointer, pointing to a new\_size-sized buffer with old contents from the buffer at mem\_ptr.
 * @retval NULL
 * - Implementation-defined, but may do so if new\_size is 0.
 * - If no memory space is available.
 */
static inline void* std_reallocate(void* context_ptr, void* mem_ptr, size_t alignment, size_t old_size, size_t new_size) {
    return realloc(mem_ptr, new_size);
}

/**
 * @brief `free` wrapper to allocate memory.
 * @note Can pass NULL as context\_ptr.
 *
 * @warning May not free the same allocated buffer twice, as this may lead to memory leaks.
 *
 * @param[in] context_ptr (unused)
 * @param[in] mem\_ptr Pointer to the memory buffer. If NULL, then no operation is done.
 */
static inline void std_deallocate(void* context_ptr, void* mem_ptr) {
    return free(mem_ptr);
}

#pragma GCC diagnostic pop

static const allocator_ops_type std_allocator_ops =
    (allocator_ops_type){.allocate_f_p = std_allocate, .reallocate_f_p = std_reallocate, .deallocate_f_p = std_deallocate};
