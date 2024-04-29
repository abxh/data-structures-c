/**
 * @file align_forward.h
 * @brief align pointer to the next alignment boundary.
 */

#pragma once

#include <stdint.h> // uintptr_t
#include <stdlib.h> // size_t

/**
 * Align pointer to the next alignment boundary.
 *
 * @param ptr The pointer as number of bytes.
 * @param align The alignment
 * @return A new pointer as number of bytes, which is aligned to `align` and is larger than or equal to `ptr`.
 */
uintptr_t align_forward(uintptr_t ptr, size_t align);
