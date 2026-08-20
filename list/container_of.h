/**
 * @file container_of.h
 * @brief container_of definition
 *
 * For more info, see:
 * @li https://radek.io/posts/magical-container_of-macro/
 * @li https://github.com/clibs/container_of
 * @li https://stackoverflow.com/questions/53840498/how-to-assert-two-types-are-equal-in-c
 * @li https://github.com/torvalds/linux/blob/master/include/linux/container_of.h
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)

/**
 * @def container_of_helper_same_type(T1, T2)
 * @brief gcc macro to check if two types are equal
 * @return boolean value
 */
#ifndef container_of_helper_same_type
#define container_of_helper_same_type(T1, T2) \
    __extension__(__builtin_types_compatible_p(__typeof__(T1), __typeof__(T2)))
#endif

/**
 * @def container_of_helper(ptr, type, member)
 * @brief Obtain a pointer to the struct that contains the member
 *
 * @param[in] ptr               Pointer to the member.
 * @param[in] type              Container type
 * @param[in] member            Member name.
 *
 * @return                      A pointer to the struct containing the member.
 */
#ifndef container_of_helper
#define container_of_helper(ptr, type, member)                                   \
    __extension__({                                                              \
        void *__mptr = (void *)(ptr);                                            \
        static_assert(container_of_helper_same_type(*(ptr), ((type *)0)->member) \
                          || container_of_helper_same_type(*(ptr), void),        \
                      "pointer type mismatch in container_of()");                \
        ((type *)(__mptr - offsetof(type, member)));                             \
    })
#endif

/**
 * @def container_of(ptr, type, member)
 * @brief Obtain a pointer to the struct that contains the member.
 *
 * @note Any const qualifier of `ptr` is discarded.
 *
 * @param[in] ptr               Pointer to the member.
 * @param[in] type              Container type.
 * @param[in] member            Member name.
 *
 * @return                      Pointer to the containing struct.
 */
#ifndef container_of
#define container_of(ptr, type, member) ((type *)container_of_helper(ptr, type, member))
#endif

/**
 * @def container_of_const(ptr, type, member)
 * @brief Obtain a pointer to the struct that contains the member,
 *        preserving the const qualifier of the pointer.
 *
 * @param[in] ptr               Pointer to the member.
 * @param[in] type              Container type.
 * @param[in] member            Member name.
 *
 * @return                      Pointer to the containing struct.
 */
#ifndef container_of_const
#define container_of_const(ptr, type, member)                                               \
    _Generic((ptr),                                                                         \
        const __typeof__(*(ptr)) *: ((const type *)container_of_helper(ptr, type, member)), \
        default: ((type *)container_of_helper(ptr, type, member)))
#endif

#else

/**
 * @def container_of(ptr, type, member)
 * @brief Obtain a pointer to the struct that contains the member.
 *
 * @note Any const qualifier of `ptr` is discarded.
 *
 * @param[in] ptr               Pointer to the member.
 * @param[in] type              Container type.
 * @param[in] member            Member name.
 *
 * @return                      Pointer to the containing struct.
 */
#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#endif

#ifdef __cplusplus
}
#endif
