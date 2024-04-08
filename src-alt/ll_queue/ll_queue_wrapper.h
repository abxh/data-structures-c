/*
    Including this header file generates static functions,
    which can be used to directly interact with values.

    The functions generated for a given value type T are:
    - ll_queue_init_T
    - ll_queue_peek_T
    - ll_queue_peek_first_T
    - ll_queue_peek_last_T
    - ll_queue_push_T
    - ll_queue_pop_T

    Note that the given types cannot include spaces because C functions
    and variables cannot either.
    Use a typedef and replace spaces with _ if needed.
*/

#include "ll_queue.h" // ll_queue_*
#include <stdbool.h>  // bool, true, false
#include <stdlib.h>   // NULL, size_t

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

static inline bool JOIN(ll_queue_init, VALUE_TYPE)(ll_queue_type** ll_queue_pp) {
    return ll_queue_init(ll_queue_pp, sizeof(VALUE_TYPE));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

static inline const VALUE_TYPE JOIN(ll_queue_peek, VALUE_TYPE)(const ll_queue_type* ll_queue_p) {
    return *(VALUE_TYPE*)(ll_queue_peek_node(ll_queue_p)->value_p);
}

static inline const VALUE_TYPE JOIN(ll_queue_peek_first, VALUE_TYPE)(const ll_queue_type* ll_queue_p) {
    return *(VALUE_TYPE*)(ll_queue_peek_first_node(ll_queue_p)->value_p);
}

static inline const VALUE_TYPE JOIN(ll_queue_peek_last, VALUE_TYPE)(const ll_queue_type* ll_queue_p) {
    return *(VALUE_TYPE*)(ll_queue_peek_last_node(ll_queue_p)->value_p);
}

#pragma GCC diagnostic pop

static inline bool JOIN(ll_queue_enqueue, VALUE_TYPE)(ll_queue_type* ll_queue_p, const VALUE_TYPE value) {

    ll_queue_node_type* node_p = ll_queue_node_create(ll_queue_p);
    if (node_p == NULL) {
        return false;
    }
    *(VALUE_TYPE*)node_p->value_p = value;
    ll_queue_enqueue_node(ll_queue_p, node_p);

    return true;
}

static inline VALUE_TYPE JOIN(ll_queue_dequeue, VALUE_TYPE)(ll_queue_type* ll_queue_p) {

    ll_queue_node_type* node_p = ll_queue_dequeue_node(ll_queue_p);
    VALUE_TYPE value = *(VALUE_TYPE*)node_p->value_p;
    ll_queue_node_free(ll_queue_p, node_p);

    return value;
}

#undef VALUE_TYPE
#undef CAT
#undef PASTE
#undef JOIN
