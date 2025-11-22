#include <stdarg.h>
#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"

// comment below line to show debug trace
// #undef DEBUG_TRACE_EXECUTION

#define STACK_BASE_CAPACITY 256

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack.values;
    // printf("stackTop reset: %p\n", (void*)vm.stackTop);
}

static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

void initVM() {
    initBaseSizeValueArray(&vm.stack, STACK_BASE_CAPACITY);
    resetStack();
}

void freeVM() {
    freeValueArray(&vm.stack);
}

void push(Value value) {
    // printf("\npush\n");
    Value* prev_stack_ptr = vm.stack.values;
    writeValueArray(&vm.stack, value);
    // printf("stack count, capacity, stack_ptr: %d, %d, %p\n", vm.stack.count, vm.stack.capacity, (void*)vm.stack.values);
    if (prev_stack_ptr != vm.stack.values) vm.stackTop = vm.stack.values + vm.stack.count - 1; // point stackTop to correct address after stack realloc
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    // printf("\npop\n");
    vm.stackTop--;
    return *vm.stackTop;
}

static Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value)); // in Lox 'nil' and 'false' are falsey, everything else is true.
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
#define BINARY_OP(valueType, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
    } while (false)
#endif

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack.values; slot < vm.stackTop; slot++) {
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
            case OP_NIL:        push(NIL_VAL); break;
            case OP_TRUE:       push(BOOL_VAL(true)); break;
            case OP_FALSE:      push(BOOL_VAL(false)); break;
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GREATER:    BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:       BINARY_OP(BOOL_VAL, <); break;
            case OP_ADD:        BINARY_OP(NUMBER_VAL, +); break;
            case OP_SUBTRACT:   BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY:   BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:     BINARY_OP(NUMBER_VAL, /); break;
            case OP_NOT:        push(BOOL_VAL(isFalsey(pop()))); break;
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
#define NEGATE_IN_PLACE 1
#if NEGATE_IN_PLACE
                *(vm.stackTop - 1) = NUMBER_VAL(-AS_NUMBER(*(vm.stackTop - 1)));
#else
                push(NUMBER_VAL(-AS_NUMBER(pop())));
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

InterpretResult interpret(const char* source) {
    // compile(source);
    // return INTERPRET_OK;
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}