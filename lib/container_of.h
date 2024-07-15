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
 * @retval A pointer to the container instance.
 */
#define container_of(ptr, type, member)                   \
    __extension__({                                       \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })
