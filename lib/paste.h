/**
 * @file paste.h
 * @brief C preprocessor macros for pasting tokens together.
 */

#pragma once

/**
 * @def PASTE(a,b)
 * @brief Paste two tokens together.
 */
#define PASTE(a, b) a##b

/**
 * @def XPASTE(a,b)
 * @brief First expand tokens, then paste them together.
 */
#define XPASTE(a, b) PASTE(a, b)

/**
 * @def JOIN(a,b)
 * @brief First expand tokens, then paste them together with a _ in between.
 */
#define JOIN(a, b) XPASTE(a, XPASTE(_, b))

// vim: ft=c
