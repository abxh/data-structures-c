/*  paste.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information. */

/**
 * @file paste.h
 * @brief C preprocessor macros for pasting tokens together
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
