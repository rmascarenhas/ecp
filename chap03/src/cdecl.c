/* cdecl.c - Translates a C declaration to English.
 *
 * The C declaration syntax can become confusing as you add type qualifiers,
 * pointers, array subscripts and functions. However, as soon as you understand
 * the process by which the declarations are parsed, you get better at
 * reading and writing them.
 *
 * This program parses C declarations and transforms them into readable English
 * correspondents. It is by no means complete and has limited error handling,
 * but is enough to enligthen some more complicated C declarations. The general
 * algorithm idea was taken from the "Expert C Programming" book.
 *
 * Usage:
 *
 * 	$ ./cdecl <declaration>
 *
 * 	declaration - the declaration to be parsed. Note that a space is required
 * 	between each token in the command line (e.g., char* is not valid).
 *
 * Author: Renato Mascarenhas
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token_stack.h"

#define PROGRAM_NAME ("cdecl")

char **chunks, **curr;

static void helpAndLeave(int status);
static void pexit(const char *fCall);
static void fatal(const char *msg, ...);
static int str_is_any(const char *str, int size, int n, ...);

static enum token_type classify_string(const char *str);
static int valid_identifier(const char *str);
static int find_identifier(struct token_stack *stack);
static int parse_declarator(struct token_stack *stack);

int
main(int argc, char *argv[]) {
	if (argc == 1)
		helpAndLeave(EXIT_FAILURE);

	struct token_stack *stack = NULL;

	chunks = &argv[1]; /* skip program name */
	curr = chunks;

	if (stack_init(&stack) == -1)
		pexit("stack_init");

	if (find_identifier(stack) == -1)
		fatal("invalid declaration: no identifier");

	if (parse_declarator(stack) == -1) {
		if (*curr)
			fatal("syntax error in declaration near %s", *curr);
		else
			fatal("syntax error in declaration");
	}

	printf("\n");
	stack_destroy(stack);

	exit(EXIT_SUCCESS);
}

static int
find_identifier(struct token_stack *stack) {
	struct token t;
	enum token_type class;

	while (*curr != NULL) {
		switch (class = classify_string(*curr)) {
			case TOKEN_IDENTIFIER:
				printf("%s is a ", *curr);
				return 0;

			case TOKEN_QUALIFIER:
			case TOKEN_TYPE:
			case TOKEN_ARRAY_BEGIN:
			case TOKEN_ARRAY_END:
			case TOKEN_FUNC_BEGIN:
			case TOKEN_FUNC_END:
				t.type = class;
				snprintf(t.string, MAXTOKENLEN, "%s", *curr);

				if (stack_push(stack, &t) == -1)
					pexit("stack_push");
				break;

			case TOKEN_UNKNOWN:
				fatal("internal error: unkown token type for %s", *curr);
				break;
		}

		++curr;
	}

	/* if we get here, no identifier was found */
	return -1;
}

static int
handle_array() {
	++curr; /* advance to read the size of the array */
	char *size = *curr;

	if (!(*curr))
		return -1;

	if (classify_string(*curr) == TOKEN_ARRAY_END) {
		/* array with no size specification */
		printf("array [] of ");
		return 0;
	}

	/* ensure that array size is a number */
	int i = 0;
	while ((*curr)[i]) {
		if (!isdigit((*curr)[i])) {
			return -1;
		}

		++i;
	}

	/* must be the closing square brackets */
	++curr;
	if (classify_string(*curr) != TOKEN_ARRAY_END)
		return -1;

	printf("array [%s] of ", size);
	++curr;
	return 0;

}

static int
handle_function() {
	enum token_type class;

	for (;;) {
		++curr;
		class = classify_string(*curr);

		if (class == TOKEN_FUNC_END) {
			printf("a function returning ");
			++curr;
			return 0;
		}

		if (class == TOKEN_UNKNOWN)
			return -1;
	}
}

static void
print_pointers(struct token_stack *stack) {
	struct token t;

	/* nothing on the stack */
	if (stack_pop(stack, &t) == -1)
		return;

	while (!strncmp(t.string, "*", MAXTOKENLEN)) {
		printf("pointer to ");
		if (stack_pop(stack, &t) == -1)
			return;
	}

	/* if we get here, we pop'ed a token that was not a pointer qualifier
	 * that still needs parsing, so we push it back again */
	stack_push(stack, &t);
}

static void
print_token(struct token t) {
	if (t.type == TOKEN_QUALIFIER) {
		if (!strncmp(t.string, "const", MAXTOKENLEN))
			printf("read-only ");
		else if (!strncmp(t.string, "*", MAXTOKENLEN))
			printf("pointer to ");
	} else {
		printf("%s ", t.string);
	}
}

static int
parse_declarator(struct token_stack *stack) {
	++curr;
	enum token_type class;
	struct token t;

	class = classify_string(*curr);
	if (class == TOKEN_ARRAY_BEGIN)
		if (handle_array() == -1)
			return -1;

	class = classify_string(*curr);
	if (class == TOKEN_FUNC_BEGIN)
		if (handle_function() == -1)
			return -1;

	print_pointers(stack);

	while (stack_pop(stack, &t) != -1) {
		if (!strncmp(t.string, "(", MAXTOKENLEN)) {
			/* expect the closing parentheses */
			if (!(*curr) || strncmp(*curr, ")", MAXTOKENLEN))
				return -1;

			parse_declarator(stack);
		} else {
			print_token(t);
		}
	}

	return 0;
}

static enum token_type
classify_string(const char *str) {
	if (!str)
		return TOKEN_UNKNOWN;

	if (!strncmp(str, "(",  MAXTOKENLEN))
		return TOKEN_FUNC_BEGIN;

	if (!strncmp(str, ")", MAXTOKENLEN))
		return TOKEN_FUNC_END;

	if (!strncmp(str, "[", MAXTOKENLEN))
		return TOKEN_ARRAY_BEGIN;

	if (!strncmp(str, "]", MAXTOKENLEN))
		return TOKEN_ARRAY_END;

	if (str_is_any(str, MAXTOKENLEN, 4, "const", "unsigned", "volatile", "*"))
		return TOKEN_QUALIFIER;

	if (str_is_any(str, MAXTOKENLEN, 6, "int", "long", "char", "float", "double", "void"))
		return TOKEN_TYPE;

	if (valid_identifier(str))
		return TOKEN_IDENTIFIER;

	return TOKEN_UNKNOWN;
}

static int
valid_identifier(const char *str) {
	if (isdigit(str[0]))
		return 0;

	int i = 0;
	while (str[i]) {
		if (!(str[i] == '_' || isalnum(str[i])))
			return 0;

		++i;
	}

	return 1;
}

static int
str_is_any(const char *str, int size, int n, ...) {
	int i, match;
	char *p;
	va_list ap;

	va_start(ap, n);
	match = 0;

	for (i = 0; i < n && !match; ++i) {
		p = va_arg(ap, char *);
		match = !strncmp(str, p, size);
	}

	va_end(ap);
	return match;
}

static void
helpAndLeave(int status) {
	FILE *stream = stderr;

	if (status == EXIT_SUCCESS)
		stream = stdout;

	fprintf(stream, "Usage: %s <declaration>\n", PROGRAM_NAME);
	exit(status);
}

static void
pexit(const char *fCall) {
	perror(fCall);
	exit(EXIT_FAILURE);
}

static void
fatal(const char *msg, ...) {
	va_list ap;
	va_start(ap, msg);

	fprintf(stderr, "%s: ", PROGRAM_NAME);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);
	exit(EXIT_FAILURE);
}
