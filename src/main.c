#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

#include <time.h>
#include <stdio.h>

int main(int argc, const char* argv[]) {
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    // int constant_idx = addConstant(&chunk, 1.2);
    // writeChunk(&chunk, OP_CONSTANT, 123);
    // writeChunk(&chunk, constant_idx, 123);
    
    // constant_idx = addConstant(&chunk, 3.4);
    // writeChunk(&chunk, OP_CONSTANT, 123);
    // writeChunk(&chunk, constant_idx, 123);
    
    // writeChunk(&chunk, OP_ADD, 123);
    
    writeConstant(&chunk, 10, 123);
    for (int i = 0; i < 10000000; i++) {
        writeConstant(&chunk, 10, 123);
        writeChunk(&chunk, OP_ADD, 123);
        // printf("count: %d\n", chunk.count);
        // printf("count of lines: %d\n", chunk.countOfLines);
        // printf("capacity: %d\n", chunk.capacity);
        // printf("capacity of lines: %d\n", chunk.capacityOfLines);
        // printf("values array count, capacity: %d, %d\n", chunk.constants.count, chunk.constants.capacity);
    }

    // constant_idx = addConstant(&chunk, 5.6);
    // writeChunk(&chunk, OP_CONSTANT, 123);
    // writeChunk(&chunk, constant_idx, 123);

    // writeChunk(&chunk, OP_DIVIDE, 123);
    // writeChunk(&chunk, OP_NEGATE, 123);

    writeChunk(&chunk, OP_RETURN, 123);

    // disassembleChunk(&chunk, "test chunk");
    clock_t start = clock();
    interpret(&chunk);
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", seconds);

    freeVM();
    freeChunk(&chunk);
    return 0;
}