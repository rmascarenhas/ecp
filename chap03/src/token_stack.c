#include "token_stack.h"

int
stack_init(struct token_stack **stack) {
	*stack = malloc(sizeof(struct token_stack));
	if (!*stack)
		return -1;
	
	(*stack)->size = 0;
	(*stack)->tokens = malloc(MAXTOKENLEN * sizeof(struct token));
	if (!(*stack)->tokens)
		return -1;

	return 0;
}

int
stack_push(struct token_stack *stack, struct token *el) {
	if (stack->size == MAXTOKENLEN) {
		errno = ENOMEM;
		return -1;
	}

	memcpy(&stack->tokens[stack->size], el, sizeof(struct token));
	++(stack->size);

	return 0;
}

int
stack_pop(struct token_stack *stack, struct token *el) {
	if (!stack || stack->size == 0) {
		errno = EINVAL;
		return -1;
	}

	--stack->size;
	if (el)
		memcpy(el, &stack->tokens[stack->size], sizeof(struct token));

	return 0;
}

int
stack_destroy(struct token_stack *stack) {
	if (!stack) {
		errno = EINVAL;
		return -1;
	}

	free(stack->tokens);
	free(stack);
	return 0;
}
