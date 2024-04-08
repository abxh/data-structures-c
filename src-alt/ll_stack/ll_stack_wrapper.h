/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value T are:
    - ll_stack_init_T
    - ll_stack_peek_T
    - ll_stack_push_T
    - ll_stack_pop_T
    and symbols defined in ll_stack.h

    Note that the given types cannot include spaces because C functions
    and variables cannot either.
    Use a typedef and replace spaces with _ if needed.
*/

#include "ll_stack.h" // ll_stack_*
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // NULL

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(ll_stack_init, VALUE_TYPE)(ll_stack_type** ll_stack_pp) {
    return ll_stack_init(ll_stack_pp, sizeof(VALUE_TYPE));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

static inline const VALUE_TYPE JOIN(ll_stack_peek, VALUE_TYPE)(const ll_stack_type* ll_stack_p) {
    return *(VALUE_TYPE*)(ll_stack_peek_node(ll_stack_p)->value_p);
}

#pragma GCC diagnostic pop

static inline bool JOIN(ll_stack_push, VALUE_TYPE)(ll_stack_type* ll_stack_p, const VALUE_TYPE value) {

    ll_stack_node_type* ll_stack_node_p = ll_stack_node_create(ll_stack_p);
    *(VALUE_TYPE*)ll_stack_node_p->value_p = value;
    ll_stack_push_node(ll_stack_p, ll_stack_node_p);

    return true;
}

static inline VALUE_TYPE JOIN(ll_stack_pop, VALUE_TYPE)(ll_stack_type* ll_stack_p) {

    ll_stack_node_type* ll_stack_node_p = ll_stack_pop_node(ll_stack_p);
    VALUE_TYPE value = *(VALUE_TYPE*)ll_stack_node_p->value_p;
    ll_stack_node_free(ll_stack_p, ll_stack_node_p);

    return value;
}

#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN
