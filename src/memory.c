#include <stdlib.h>
#include <stdio.h>

#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer, newSize);
    if (result == NULL) {
        // printf("oldsize requested: %ld\n", oldSize);
        // printf("newsize requested: %ld\n", newSize);
        // printf("No more MEM available!\n");
        exit(1);
    }
    return result;
}