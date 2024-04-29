/**
 * @file std_allocator.h
 * @brief wrapper of *thead-safe* standard allocation functions for conforming
 *        to the defined struct interface of allocator operations.
 *
 * Refer to following pages for more extensive documentation:
 *   @li https://man7.org/linux/man-pages/man3/malloc.3.html
 *   @li https://man7.org/linux/man-pages/man3/realloc.3p.html
 */

#pragma once

#include <stdlib.h> // size_t, malloc, realloc, free

#include "allocator_ops.h" // allocator_ops_type

/**
 * @brief `malloc` wrapper to allocate memory.
 * @note Can pass NULL as context_ptr.
 *
 * @param[in] context_ptr (unused)
 * @param[in] alignment (unused)
 * @param[in] size Amount of memory to allocate in bytes.
 * @return The stack pointer.
 * @retval NULL
 *   @li Implementation-defined, but may do so if size is 0.
 *   @li If no memory space is available.
 */
void* std_allocate(void* context_ptr, size_t alignment, size_t size);

/**
 * @brief `realloc` wrapper to allocate memory.
 * @note Can pass NULL as context_ptr.
 *
 * @warning Passing memory_ptr not allocated by malloc, calloc or free is undefined behaviour.
 *
 * @param[in] context_ptr (unused)
 * @param[in, out] mem_ptr Pointer to the memory buffer. If NULL, then malloc(size) is called.
 * @param[in] alignment (unused)
 * @param[in] old_size (unused)
 * @param[in] new_size Amount of memory to allocate in bytes.
 * @return A new pointer, pointing to a new_size-sized buffer with old contents from the buffer at mem_ptr.
           If the new pointer is not NULL, then the new pointer shall be used in place of mem_ptr.
 * @retval NULL
 *   @li Implementation-defined, but may do so if new_size is 0.
 *   @li If no memory space is available.
 */
void* std_reallocate(void* context_ptr, void* mem_ptr, size_t alignment, size_t old_size, size_t new_size);

/**
 * @brief `free` wrapper to allocate memory.
 * @note Can pass NULL as context_ptr.
 *
 * @warning May not free the same allocated buffer twice in a row, as this may lead to memory leaks.
 *
 * @param[in] context_ptr (unused)
 * @param[in, out] mem_ptr Pointer to the memory buffer to be flagged as free. If NULL, then no operation is done.
 */
void std_deallocate(void* context_ptr, void* mem_ptr);

/**
 * @brief Static `allocator_ops_type`-typed struct with preset standard allocation operations for convienience.
 */
static const allocator_ops_type std_allocator_ops =
    (allocator_ops_type){.allocate_f_ptr = std_allocate, .reallocate_f_ptr = std_reallocate, .deallocate_f_ptr = std_deallocate};
