#include "word_list.h"

#define ErrorCase(condition, err, ret_val) { \
	if (condition) { \
		errno = err; \
		return ret_val; \
	} \
}

int
word_list_init(struct word_list *wl, long size)
{
	ErrorCase(wl == NULL, EINVAL, -1);
	ErrorCase(size <= 0, EINVAL, -1);

	wl->size = size;
	wl->num_words = 0;

	wl->words = malloc(size * sizeof(char *));
	ErrorCase(wl->words == NULL, errno, -1);

	return 0;
}

int word_list_remove_at(struct word_list *wl, long p)
{
	ErrorCase(p <= 0 || p >= wl->num_words, EINVAL, -1);

	long i;
	for (i = p; i < wl->num_words - 1; ++i)
		strncpy(wl->words[i], wl->words[i + 1], WORD_LIST_LARGEST_NOUN);

	free(wl->words[wl->num_words - 1]);
	--wl->num_words;

	return 0;
}

int
word_list_append(struct word_list *wl, const char *word)
{
	return word_list_add_at(wl, word, wl->num_words);
}

int
word_list_add_at(struct word_list *wl, const char *word, long p)
{
	ErrorCase(wl->num_words >= wl->size, ENOMEM, -1);
	wl->words[wl->num_words] = malloc(WORD_LIST_LARGEST_NOUN);
	ErrorCase(wl->words[wl->num_words] == NULL, errno, -1);

	long i;
	for (i = wl->num_words - 1; i >= p; --i)
		strncpy(wl->words[i + 1], wl->words[i], WORD_LIST_LARGEST_NOUN);

	int len = strlen(word) + 1;
	ErrorCase(len >= WORD_LIST_LARGEST_NOUN, EINVAL, -1);

	/* do not copy \n if present (i.e., when data comes from a data file
	 * read with `fgets(3)` */
	if (word[len - 2] == '\n') {
		strncpy(wl->words[p], word, len - 1);
		wl->words[p][len - 2] = '\0';
	} else {
		strncpy(wl->words[p], word, len);
	}

	++wl->num_words;

	return 0;
}

int
word_list_load(struct word_list *wl, FILE *stream)
{
	ErrorCase(wl == NULL, EINVAL, -1);
	ErrorCase(stream == NULL, errno, -1);

	char *word = malloc(WORD_LIST_LARGEST_NOUN);

	while (fgets(word, WORD_LIST_LARGEST_NOUN, stream) != NULL) {
		ErrorCase(word_list_append(wl, word) == -1, errno, -1);
	}

	free(word);
	return 0;
}

/* Decides which article should precede a given word. No complex English rules are
 * embedded in here: the algorithm simply checks whether the first letter of the
 * given word is a vowel or not. */
static void
infer_article(const char *word, char *buf)
{
	switch (tolower(word[0])) {
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'u':
			strncpy(buf, "an", 3);
			break;
		default:
			strncpy(buf, "a", 2);
	}
}

int
word_list_traverse(struct word_list *wl, int (*fn)(const char *word, char *article, long p, long total))
{
	ErrorCase(wl == NULL, EINVAL, -1);

	long i;
	int retval;
	char article[3];

	for (i = 0; i < wl->num_words; ++i) {
		infer_article(wl->words[i], article);

		retval = fn(wl->words[i], article, i, wl->num_words);
		if (retval != 0)
			return retval;
	}

	return 0;
}

int
word_list_rlookup(struct word_list *wl, bool (*selector)(const char *word, char *article), char *buffer, char *article)
{
	long chosen[WORD_LIST_LOOKUP_RSET], nchosen, tries;
	long i, selected_idx;
	char _article[3], *selected_word;
	bool started_tries;

	nchosen = tries = i = 0;
	started_tries = false;
	while (nchosen < WORD_LIST_LOOKUP_RSET && tries < WORD_LIST_LOOKUP_TRIES && i < wl->num_words) {
		infer_article(wl->words[i], _article);
		if (selector(wl->words[i], _article)) {
			started_tries = true;
			chosen[nchosen] = i;
			++nchosen;
		}

		if (started_tries)
			++tries;

		++i;
	}

	if (!started_tries)
		return -1;

	selected_idx = chosen[rand() % nchosen];
	printf(">> nchosen=%ld idx=%ld rand=%ld\n", nchosen, selected_idx, rand() % nchosen);
	selected_word = wl->words[selected_idx];
	infer_article(selected_word, _article);

	strncpy(buffer, selected_word, strlen(selected_word) + 1);
	strncpy(article, _article, 3);

	return selected_idx;
}

int
word_list_destroy(struct word_list *wl)
{
	ErrorCase(wl == NULL, EINVAL, -1);

	long i;
	for (i = 0; i < wl->num_words; ++i) {
		free(wl->words[i]);
	}

	free(wl->words);
	return 0;
}
