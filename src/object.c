#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType)  (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;

    object->next = vm.objects;
    vm.objects = object;
    return object;
}

static ObjString* allocateString(char* chars, int length) {
    // ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    Obj* object = (Obj*)reallocate(NULL, 0, sizeof(ObjString) + length * sizeof(char));
    object->type = OBJ_STRING;
    object->next = vm.objects;
    vm.objects = object;
    ObjString* string = (ObjString*)object;

    string->length = length;
    // string->chars = chars;
    memcpy(string->chars, chars, length);
    return string;
}

ObjString* takeString(char* chars, int length) {
    return allocateString(chars, length);
}

ObjString* copyString(const char* chars, int length) {
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:    printf("%s", AS_CSTRING(value)); break;
    }
}