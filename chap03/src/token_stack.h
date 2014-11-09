#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAXTOKENLEN (128)
#define MAXTOKENS (256)

enum token_type { 
	TOKEN_TYPE,
	TOKEN_QUALIFIER,
	TOKEN_IDENTIFIER,
	TOKEN_ARRAY_BEGIN,
	TOKEN_ARRAY_END,
	TOKEN_FUNC_BEGIN,
	TOKEN_FUNC_END,
	TOKEN_UNKNOWN
};

struct token {
	enum token_type type;
	char string[MAXTOKENLEN];
};

struct token_stack {
	struct token *tokens;
	int size;
};

/* stack related utility functions: all of them return a nonnegative value on success
 * or -1 on error, with errno appropriately set */
int stack_init(struct token_stack **stack);
int stack_push(struct token_stack *stack, struct token *el);
int stack_pop(struct token_stack *stack, struct token *el);
int stack_destroy(struct token_stack *stack);
