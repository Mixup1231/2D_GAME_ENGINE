#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"

/***************************************************************/
Stack stack_create(size_t length, size_t item_size) {
	Stack stack = {
		.item_size = item_size,
		.length = length,
		.top = 0
	};
	stack.items = malloc(length * item_size);
	assert(stack.items);

	return stack;
}

void stack_push(Stack* stack, void* item) {
	assert(stack && item && stack->top < stack->length);

	stack->top++;
	memcpy((char*)stack->items + (stack->top - 1) * stack->item_size, item, stack->item_size);
}

void* stack_pop(Stack* stack) {
	assert(stack && stack->top != 0);

	void* top = (char*)stack->items + (stack->top - 1) * stack->item_size;
	stack->top--;
	return top;
}
/***************************************************************/