/*
    Pointer offsets (P):
    P := 0
    ...
    P := 128

    Alignments (A):
    A := 1
    A := 2
    A := 4
    A := 8

    align_forward returns either P, if (P % A) is 0,
    or P + A - (P % A) otherwise.
*/

#include "arena.h"

#include <stddef.h>
#include <stdint.h>

int main(void)
{
    uintptr_t* P = calloc(129, sizeof(uintptr_t));
    for (size_t i = 0; i < 129; i++) {
        P[i] = i;
    }

    uintptr_t A[] = {1, 2, 4, 8};

    for (size_t i = 0; i < 129; i++) {
        for (size_t j = 0; j < sizeof(A) / sizeof(*A); j++) {
            if (P[i] % A[j] == 0) {
                assert(internal_align_forward(P[i], A[j]) == P[i]);
            }
            else {
                assert(internal_align_forward(P[i], A[j]) == P[i] + A[j] - (P[i] % A[j]));
            }
        }
    }

    free(P);
}
