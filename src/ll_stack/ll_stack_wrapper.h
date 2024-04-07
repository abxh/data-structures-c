/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type t with name T are:
    - ll_stack_init_T
    - ll_stack_peek_T
    - ll_stack_push_T
    - ll_stack_pop_T

    A distinction between the type and name is made as the type
    cannot include spaces.
*/

#include "ll_stack.h" // ll_stack_*
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // NULL

#ifndef VALUE_NAME
#error "Must declare VALUE_NAME. Defaulting to 'int'."
#define VALUE_NAME int
#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(ll_stack_init, VALUE_NAME)(ll_stack_type** ll_stack_pp) {
    return ll_stack_init(ll_stack_pp, sizeof(VALUE_TYPE));
}

static inline VALUE_TYPE JOIN(ll_stack_peek, VALUE_NAME)(const ll_stack_type* ll_stack_p) {
    return *(VALUE_TYPE*)(ll_stack_peek_node(ll_stack_p)->value_p);
}

static inline bool JOIN(ll_stack_push, VALUE_NAME)(ll_stack_type* ll_stack_p, const VALUE_TYPE value) {

    ll_stack_node_type* ll_stack_node_p = ll_stack_node_create(ll_stack_p);
    *(VALUE_TYPE*)ll_stack_node_p->value_p = value;
    ll_stack_push_node(ll_stack_p, ll_stack_node_p);

    return true;
}

static inline VALUE_TYPE JOIN(ll_stack_pop, VALUE_NAME)(ll_stack_type* ll_stack_p) {

    ll_stack_node_type* ll_stack_node_p = ll_stack_pop_node(ll_stack_p);
    VALUE_TYPE value = *(VALUE_TYPE*)ll_stack_node_p->value_p;
    ll_stack_node_free(ll_stack_p, ll_stack_node_p);

    return value;
}

#undef VALUE_TYPE
#undef VALUE_NAME

#undef CAT
#undef PASTE
#undef JOIN
