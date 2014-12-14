/* panandrome.c - construct arbitrarily large palindromes.
 *
 * One of the most famous palindromes (sequences of words that remain the same
 * if read backwards) known is related to the construction of the Panama canal:
 * "A man, a plan, a canal - Panama!".
 *
 * Starting from that phrase, it is possible to create arbitrarily large
 * palindrome sentences, adding an increasing list of nouns to the description
 * of the Panama canal. The algorithm was based on the description of the
 * problem presented on the "Expert C Programming" book by Peter van der Linden.
 *
 * This program is able to generate a very large palindrome sentence based
 * on the previous idea and on a list of English nouns, to be passed as an
 * argument. The number of words in the generated palindrome can also passed
 * on the command line. Note that the length of the generated sentence is,
 * however, limited to the number of nouns present on the list given to
 * this program.
 *
 * Usage:
 *
 * 	$ ./panandrome <nouns_list> [<palindrome_words>]
 *
 * 	nouns_list - a text file of English nouns, with one word per line.
 * 	palindrome_size - the number of words the generated palindrome is to
 * 	                  contain. If not specified, a default of 10 is assumed.
 *
 * NOTE: this program is incomplete, and may not work correctly in some situations.
 * While it served me a good purpose of learning and working through a cool problem,
 * I advise anyone interested in checking Peter Norvig's take on the challenge and
 * solution - http://norvig.com/palindrome.html.
 *
 * Author: Renato Mascarenhas
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "word_list.h"

static char *progname = "panandrome";
static char state[WORD_LIST_LARGEST_NOUN]; /* the part that do not fit the palindrome */
static char previous_state[WORD_LIST_LARGEST_NOUN]; /* allows rollback */

enum palindrome_direction { LEFT, RIGHT };

static void usage(void);
static long palindrome_size(const char *arg);
static void pexit(const char *fname);

/* initializes a given `palindrome` with the short default of
 * "A man, a plan, a canal - Panama!" */
static void initialize_palindrome(struct word_list *palindrome);
static int print_palindrome(const char *word, char *article, long pos, long total);
static bool (*word_selector(enum palindrome_direction dir))(const char *word, char *article);
static bool is_palindrome(const char *word);
static void change_state(const char *word, char *article, enum palindrome_direction direction);

int main(int argc, char *argv[])
{
	if (argc < 2)
		usage();

	long size = palindrome_size(argv[2]), total, curpos, lastpos;
	struct word_list nouns, palindrome;
	char curword[WORD_LIST_LARGEST_NOUN], article[3];
	char direction;

	srand(time(NULL));

	/* assuming a reasonably large nouns list. If you use a different list
	 * than the one provided with this source file, then you might need to
	 * change this value*/
	if (word_list_init(&nouns, 150000) == -1)
		pexit("word_list_init");
	printf(">> Initialized nouns list\n");

	if (word_list_init(&palindrome, 100 * size) == -1)
		pexit("word_list_init");
	printf(">> Initialized palindrome list\n");

	FILE *db = fopen(argv[1], "r");
	if (db == NULL)
		pexit("fopen");

	if (word_list_load(&nouns, db) == -1)
		pexit("word_list_load");
	printf(">> Loaded nouns into memory\n");

	initialize_palindrome(&palindrome);
	printf(">> Built starting palindrome\n");

	strncpy(state, "aca", 4);
	strncpy(previous_state, "aca", 4);
	direction = LEFT;
	curpos = 2; /* first added word should be at the left of position 2 */
	lastpos = curpos;
	total = 4;  /* starting palindrome size */

	printf(">> Main loop will start: state=%s total=%ld position=%ld size=%ld\n", state, total, curpos, size);
	/* main palindrome generation loop */
	while (total < size || !is_palindrome(state)) {
		if (word_list_rlookup(&nouns, word_selector(direction), curword, article) == -1) {
			word_list_remove_at(&palindrome, lastpos);
			strncpy(state, previous_state, WORD_LIST_LARGEST_NOUN);
			--total;
			direction = (direction == LEFT ? RIGHT : LEFT);
			continue;
			/* fprintf(stderr, "%s: no available words for a %ld words long palindrome\n", progname, size); */
			/* exit(EXIT_FAILURE); */
		}

		if (word_list_add_at(&palindrome, curword, curpos) == -1)
			pexit("word_list_add_at");

		change_state(curword, article, direction);

		lastpos = curpos;
		if (direction == RIGHT)
			++curpos;

		++total;
		direction = (direction == LEFT ? RIGHT : LEFT);
		printf(">> After loop: word=%s article=%s state=%s direction=%s position=%ld total=%ld\n", curword, article, state, direction == LEFT ? "LEFT" : "RIGHT", curpos, total);
	}
	printf(">> Main loop finished\n");

	/* print generated palindrome */
	word_list_traverse(&palindrome, print_palindrome);
	printf("\n");

	word_list_destroy(&nouns);
	word_list_destroy(&palindrome);

	exit(EXIT_SUCCESS);
}

static void
initialize_palindrome(struct word_list *palindrome)
{
	word_list_append(palindrome, "man");
	word_list_append(palindrome, "plan");
	word_list_append(palindrome, "canal");
	word_list_append(palindrome, "Panama");
}

static bool
left_selector(const char *word, char *article)
{
	char comparable[WORD_LIST_LARGEST_NOUN];
	snprintf(comparable, WORD_LIST_LARGEST_NOUN, "%s%s", article, word);

	return (strncmp(comparable, state, strlen(state)) == 0);
}

static bool
right_selector(const char *word, char *article)
{
	/* we have to make sure that the chosen word ends either with 'a' or 'na'
	 * (reverse of 'an') in order to allow for a new word to be found, since
	 * an article should precede every word. */
	char ending1[WORD_LIST_LARGEST_NOUN],
	     ending2[WORD_LIST_LARGEST_NOUN],
	     comparable[WORD_LIST_LARGEST_NOUN];

	size_t ending1_len, ending2_len, comparable_len;

	/* if the state already starts with 'a'  or 'na', do nothing */
	if (state[0] == 'a' || !strncmp(state, "na", 2)) {
		snprintf(ending1, WORD_LIST_LARGEST_NOUN, "%s", state);
		snprintf(ending1, WORD_LIST_LARGEST_NOUN, "%s", state);
	} else {
		snprintf(ending1, WORD_LIST_LARGEST_NOUN, "a%s", state);
		snprintf(ending2, WORD_LIST_LARGEST_NOUN, "na%s", state);
	}
	snprintf(comparable, WORD_LIST_LARGEST_NOUN, "%s%s", article, word);

	ending1_len = strlen(ending1);
	ending2_len = strlen(ending2);
	comparable_len = strlen(comparable);

	/* our comparable needs to end with either ending1 or ending2. */
	return ((strncmp(&(comparable[comparable_len - ending1_len]), ending1, ending1_len) == 0) ||
			strncmp(&(comparable[comparable_len - ending2_len]), ending1, ending2_len) == 0);

}

static bool
(*word_selector(enum palindrome_direction dir))(const char *word, char *article)
{
	return dir == LEFT ? left_selector : right_selector;
}

/* reverses a string in place */
static void
reverse(char *word)
{
	size_t len = strlen(word),
	       i, j;
	char c;

	i = 0; j = len - 1;

	while (i < j) {
		c = word[j];
		word[j] = word[i];
		word[i] = c;

		++i; --j;
	}
}

/* changes the algorithm `state` by removing the current state from the union
 * of the article and the chosen word, acccording to the direction that it is
 * being added to the palindrome */
static void
change_state(const char *word, char *article, enum palindrome_direction direction)
{
	size_t s = WORD_LIST_LARGEST_NOUN + 3, /* account for article size */
	       statelen = strlen(state);
	char token[s];

	strncpy(previous_state, state, statelen + 1); /* backup state */
	snprintf(token, s, "%s%s", article, word);

	if (direction == LEFT) {
		/* state will be same as token, after removing the first letters,
		 * according to the current state length */
		strncpy(state, &(token[statelen]), WORD_LIST_LARGEST_NOUN);
	} else {
		/* in a similar logic, the new state should be equal to `token`,
		 * after removing as many letters as we currently have on state */
		token[strlen(token) - statelen + 1] = '\0';
		strncpy(state, token, WORD_LIST_LARGEST_NOUN);
	}

	/* the state should be reversed on every iteration in order to generate
	 * a palindrome */
	reverse(state);
}

static int
print_palindrome(const char *word, char *article, long pos, long total)
{
	char prefix[3];

	/* for the first word in the palindrome, the article should be uppercased,
	 * and not prefixed with a comma; in case it is the last word (Panama), there
	 * should be no comma, but a dash instead. */
	if (pos == 0) {
		article[0] = toupper(article[0]);
		prefix[0] = '\0';
	} else if (pos == total - 1) {
		strncpy(prefix, " -", 3);
	} else {
		strncpy(prefix, ", ", 3);
	}

	printf("%s", prefix);
	if (pos != total - 1)
		printf("%s", article);
	printf(" %s", word);

	/* end with an exciting exclamation mark! */
	if (pos == total - 1)
		printf("!");

	return 0;
}

static void
usage()
{
	fprintf(stderr, "Usage: %s <nouns_list> [<palindrome_size>]\n", progname);
	exit(EXIT_FAILURE);
}

static bool
is_palindrome(const char *word)
{
	size_t len = strlen(word);
	size_t i, j;

	i = 0; j = len - 1;
	while (i < j) {
		if (word[i] != word[j])
			return false;

		++i; --j;
	}

	return true;
}

static long
palindrome_size(const char *arg)
{
	if (arg) {
		long s;
		char *endptr;

		s = strtol(arg, &endptr, 10);
		if (endptr == arg || s <= 0) {
			fprintf(stderr, "%s: %s: invalid palindrome size (must be >= 4)\n", progname, arg);
			exit(EXIT_FAILURE);
		}

		return s;
	} else {
		return 10;
	}
}

static void
pexit(const char *fname)
{
	perror(fname);
	exit(EXIT_FAILURE);
}
