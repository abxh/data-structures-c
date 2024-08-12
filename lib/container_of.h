/*  container_of.h
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
 * @file container_of.h
 * @brief container_of definition
 *
 * See:
 * https://radek.io/posts/magical-container_of-macro/
 */

#pragma once

/**
 * @def container_of(ptr, type, member)
 * @brief Get the pointer to the container instance, given a pointer to
 *        to the member instance, container type, and member name.
 *
 * @param[in] ptr A pointer to the member.
 * @param[in] type The container type.
 * @param[in] member The member name.
 *
 * @return A pointer to the container instance.
 */
#define container_of(ptr, type, member)                   \
    __extension__({                                       \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })
