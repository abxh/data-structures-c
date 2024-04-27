/**
 * @file join.h
 * @brief C preprocessor macros to paste tokens together.
 *
 * Why these macros are defined?
 *
 * These are used to create the symbols (struct names / function names) for different data
 * structure types.
 *
 * Alternatives:
 * - `void*` types, which are converted at runtime; Or byte arrays with a data_size worked with at runtime.
 * - Seperate data structure type code for each type used.
 * - Macros expanded with type parameters.
 */

#pragma once

/**
 * @def PASTE(a,b)
 * @brief Pastes it's two parameters together.
 */
#define PASTE(a, b) a##b

/**
 * @def CONCAT(a,b)
 * @brief First expands it's two parameters, then pastes them together.
 */
#define CONCAT(a, b) PASTE(a, b)

/**
 * @def JOIN(a,b)
 * @brief First expands it's two parameters, then pastes them together with a underscore _ in between.
 */
#define JOIN(a, b) CONCAT(a, CONCAT(_, b))
