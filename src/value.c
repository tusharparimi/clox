#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "value.h"

// #define VALUE_ARRAY_BASE_CAPACITY 8

void initValueArray(ValueArray* array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void initBaseSizeValueArray(ValueArray* array, int baseSize) {
    array->values = malloc(baseSize * sizeof(Value));
    array->capacity = baseSize;
    array->count = 0;
}

void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        // array->capacity = GROW_CAPACITY(oldCapacity, VALUE_ARRAY_BASE_CAPACITY);
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    printf("%g", value);
}