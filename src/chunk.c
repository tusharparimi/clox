#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    chunk->countOfLines = 0;
    chunk->capacityOfLines = 0;
    initValueArray(&chunk->constants);
}

void writeLine(int* lines, int line, int* count) {
    if (*count > 1 && lines[*count - 2] == line) {
        lines[*count - 1]++;
    }
    else {
        lines[*count] = line;
        lines[*count + 1] = 1;
        *count += 2;
    }
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    if (chunk->capacityOfLines < chunk->countOfLines + 1) {
        int oldCapacity = chunk->capacityOfLines;
        chunk->capacityOfLines = GROW_CAPACITY(oldCapacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacityOfLines);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
    writeLine(chunk->lines, line, &chunk->countOfLines);
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}