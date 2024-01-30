#include <stdio.h>
#include <stdint.h>

typedef struct {
    void** darr;
    size_t size;
    size_t capacity;
} Dict;


