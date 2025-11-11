#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);

    int constant_idx = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant_idx, 123);

    for (int i = 0; i < 260; i++) {
        writeConstant(&chunk, i, 124);
    }
    writeChunk(&chunk, OP_RETURN, 125);

    disassembleChunk(&chunk, "test chunk");

    freeChunk(&chunk);
    return 0;
}