#include <stdio.h>

#define ST_VALUE double
#include "stack.h"

int main() {
    stack *stack_p = st_new();

    st_push(stack_p, 1.);
    printf("%f\n", stack_p->head->value);

    st_free(stack_p);
    return 0;
}
