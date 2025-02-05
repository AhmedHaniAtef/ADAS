#include <my_stack.h>
#include <stdlib.h>

// Create a new stack
Stack *stack_create() {
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (!stack) return NULL;

    stack->list = list_create();
    if (!stack->list) {
        free(stack);
        return NULL;
    }
    return stack;
}

// Destroy the stack
void stack_destroy(Stack *stack, void (*free_data)(void *)) {
    if (!stack) return;

    list_destroy(stack->list, free_data);
    free(stack);
}

// Push an element onto the stack
void stack_push(Stack *stack, void *data) {
    if (!stack) return;

    list_append(stack->list, data);
}

// Pop an element from the stack
void *stack_pop(Stack *stack) {
    if (!stack) return NULL;

    return list_remove_tail(stack->list);
}

// Get the size of the stack
size_t stack_size(const Stack *stack) {
    return stack ? list_size(stack->list) : 0;
}
