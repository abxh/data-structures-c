/*  container_of.h
 *
 *  Copyright (C) 2023 abfish
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

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
 * @return A pointer to the container instance.
 */
#define container_of(ptr, type, member)                   \
    __extension__({                                       \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })
