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

void writeConstant(Chunk* chunk, Value value, int line) {
    uint32_t constant_idx = addConstant(chunk, value);
    if (constant_idx < 256) {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, constant_idx, line);
    }
    else {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        uint8_t b0 = (constant_idx >> 16) & 0xFF;
        uint8_t b1 = (constant_idx >> 8) & 0xFF;
        uint8_t b2 = constant_idx & 0xFF;
        writeChunk(chunk, b0, line);
        writeChunk(chunk, b1, line);
        writeChunk(chunk, b2, line);
    } 
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