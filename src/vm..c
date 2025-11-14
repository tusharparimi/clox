#include <stdio.h>

#include "common.h"
#include "vm.h"
#include "debug.h"

// comment below line to show debug trace
#undef DEBUG_TRACE_EXECUTION

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {

}

void push(Value value) {
    // printf("\npush\n");
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    // printf("\npop\n");
    vm.stackTop--;
    return *vm.stackTop;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

#define MOVE_STACK_POINTER_LESS 0
#if MOVE_STACK_POINTER_LESS
#define BINARY_OP(op) \
    do { \
        double b = pop(); \
        *(vm.stackTop - 1) = *(vm.stackTop - 1) op b; \
    } while (false)
#else
#define BINARY_OP(op) \
    do { \
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } while (false)
#endif

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_CONSTANT_LONG: {
                uint32_t constant_idx = 0;
                for (int i = 2; i > -1; i--) {
                    uint8_t byte = READ_BYTE();
                    constant_idx = constant_idx | ((uint32_t)byte << 8*i);
                }
                Value constant = vm.chunk->constants.values[constant_idx];
                push(constant);
                break;
            }
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_NEGATE: {
#define NEGATE_IN_PLACE 1
#if NEGATE_IN_PLACE
                *(vm.stackTop - 1) = -*(vm.stackTop - 1);
#else
                push(-pop());
#endif
#undef NEGATE_IN_PLACE
                break;
            }
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
#undef MOVE_STACK_POINTER_LESS
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}