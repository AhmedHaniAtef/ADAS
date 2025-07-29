#ifndef MY_STACK_H
#define MY_STACK_H

#include "my_list.h"

typedef struct Stack {
    List *list;
} Stack;

// Function prototypes
Stack *stack_create();
void stack_destroy(Stack *stack, void (*free_data)(void *));
void stack_push(Stack *stack, void *data);
void *stack_pop(Stack *stack);
size_t stack_size(const Stack *stack);

#endif // MY_STACK_H
