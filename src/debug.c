#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
    uint32_t constant = 0;
    int next_offset;
    if (name == "OP_CONSTANT") {
        constant = chunk->code[offset + 1];
        next_offset = offset + 2;
    }
    else {
        uint8_t b0 = chunk->code[offset + 1];
        uint8_t b1 = chunk->code[offset + 2];
        uint8_t b2 = chunk->code[offset + 3];
        constant = ((uint32_t)b0 << 16) | ((uint32_t)b1 << 8) | (uint32_t)b2;
        next_offset = offset + 4;
    }
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return next_offset;
}

static int getLine(int* lines, int offset) {
    int i = 1;
    int count = 0;
    while (count <= offset + 1) {
        count += lines[i];
        if (count > offset) {
            return lines[i - 1];
        }
        i += 2;
    }
    exit(1);
}

int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);
    if (offset > 0 && getLine(chunk->lines, offset) == getLine(chunk->lines, offset - 1)) {
        printf("   | ");
    }
    else {
        printf("%4d ", getLine(chunk->lines, offset));
    }
    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return constantInstruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_ADD:
            return simpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}