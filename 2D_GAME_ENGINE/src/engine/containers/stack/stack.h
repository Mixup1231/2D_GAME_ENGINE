#pragma once
#include <stddef.h>

typedef struct stack {
	void* items;
	size_t item_size;
	size_t length;
	size_t top;
} Stack;

Stack stack_create(size_t length, size_t item_size);
void stack_push(Stack* stack, void* item);
void* stack_pop(Stack* stack);