/*
    fixed-size array-based stack

    The following macros are defined:
    - astack_for_each

    The following structs are defined:
    - astack_T

    The following functions are generated for a given value type t with label T:
    - astack_T_init
    - astack_T_deinit
    - astack_T_count
    - astack_T_is_empty
    - astack_T_is_full
    - astack_T_peek
    - astack_T_push
    - astack_T_pop

    A distinction between the value and label is made as the label
    cannot include spaces.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#ifndef VALUE_LABEL
#error "Must declare VALUE_LABEL. Defaulting to 'int'."
#define VALUE_LABEL int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define astack_T JOIN(astack, VALUE_LABEL)
#define astack_T_init JOIN(astack_T, init)
#define astack_T_deinit JOIN(astack_T, deinit)
#define astack_T_count JOIN(astack_T, count)
#define astack_T_is_empty JOIN(astack_T, is_empty)
#define astack_T_is_full JOIN(astack_T, is_full)
#define astack_T_peek JOIN(astack_T, peek)
#define astack_T_push JOIN(astack_T, push)
#define astack_T_pop JOIN(astack_T, pop)



#undef astack_T 
#undef astack_T_init 
#undef astack_T_deinit 
#undef astack_T_count 
#undef astack_T_is_empty 
#undef astack_T_peek 
#undef astack_T_push 
#undef astack_T_pop 

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_LABEL
#undef VALUE_TYPE

#endif
