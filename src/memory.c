#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "vm.h"

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

static void freeObject(Obj* object) {
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            // FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object); // when using flexible array member freeing struct object is enough
            break;
        }
    }
}

void freeObjects() {
    // int count = 0;
    Obj* object = vm.objects;
    while (object != NULL) {
        // printf("count: %d\n", count++);
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}