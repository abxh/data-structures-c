/**
 * @file allocator_ops.h
 * @brief struct interface for allocator operations.
 */

#pragma once

#include <stdlib.h> // size_t

/**
 * @brief struct interface for allocator operations.
 */
typedef struct {
    void* (*allocate_f_p)(void* context_ptr, size_t alignment, size_t size); ///< allocate memory.
    void* (*reallocate_f_p)(void* context_ptr, void* mem_ptr, size_t alignment, size_t old_size,
                            size_t new_size);                 ///< reallocate memory.
    void (*deallocate_f_p)(void* context_ptr, void* mem_ptr); ///< deallocate memory.
} allocator_ops_type;
