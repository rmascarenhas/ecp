/* wordlist - allows loading large word lists and perform lookups
 * on it based on prefix, suffix, or absolute match. */

#ifndef WORD_LIST_H
#define WORD_LIST_H

#ifndef WORD_LIST_LARGEST_NOUN
#  define WORD_LIST_LARGEST_NOUN (64)
#endif

/* by default, generate a sample of 10 valid words when performing an rlookup,
 * or give up after 50 tries */
#ifndef WORD_LIST_LOOKUP_RSET
#  define WORD_LIST_LOOKUP_RSET (100)
#endif

#ifndef WORD_LIST_LOOKUP_TRIES
#  define WORD_LIST_LOOKUP_TRIES (500)
#endif

#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

struct word_list {
	long size;      /* maximum number of words allowed in this list */
	long num_words; /* number of words loaded in the struct */
	char **words;   /* list of NUL-terminated strings */
};

/* initializes a previously allocated `word_list` struct. The struct will support
 * a maximum of `size` words in it.
 *
 * Returns a positive number on success, -1 on error */
int word_list_init(struct word_list *wl, long size);

/* appends a given `word` to the word list. The passed buffer is not modified
 * and can be later changed without affecting the list structure. */
int word_list_append(struct word_list *wl, const char *word);

/* removes the word at the specified position `p` from the word list */
int word_list_remove_at(struct word_list *wl, long p);

/* adds a new word to the list after a given position `p`. */
int word_list_add_at(struct word_list *wl, const char *word, long p);

/* loads the contents of the file given on `stream` into the previously
 * initialized word list. The file must contain one word per-line and must
 * be alphabetically sorted. These are hard requirements - the lookup function
 * relies on the fact that the list is ordered in order to speed up the search.
 *
 * Note that an article is added prior to each loaded noun. That is, if the
 * noun starts with a vowel, the "an" article is assumed; or "a" otherwise.
 *
 * Returns a positive number on succes or -1 on error, with `errno` set
 * appropriately. */
int word_list_load(struct word_list *wl, FILE *stream);

/* performs a lookup of a given word according to the results of the passed `selector`.
 * In case the selector returns `true`, then the search will proced to the following
 * words until `WORD_LIST_LOOKUP_RSET` words that pass the criteria are found, or
 * more than `WORD_LIST_LOOKUP_TRIES` are searched. A random word from the set that
 * passed the `comparator` test will be chosen and copied to the given `buffer`,
 * alongside the companion `article`. In that sense, the return of this function is
 * not deterministic (assuming a perfectly random function is available, which is
 * beyond the scope here).
 *
 * In case no word that matches the criteria is found, -1 is returned and the buffer
 * is not modified. */
int word_list_rlookup(struct word_list *wl, bool (*comparator)(const char *word, char *article), char *buffer, char *article);

/* traverses the word list, calling the specified callback `fn` for each word on
 * the list. The callback receives as arguments the current word, the related article
 * ('a' or 'an'), the position that word occupies on the list, and the total
 * number of words on the list. If the callback function returns a non-zero value,
 * traversal is halted and that value is returned. */
int word_list_traverse(struct word_list *wl, int (*fn)(const char *word, char *article, long p, long total));

/* destroys the given word list, removing all memory taken by previously loaded
 * lists. If the same `struct` is to be used for another list, it should be
 * given to the `word_list_init` function beforehand.
 *
 * Returns a positive number on success, or -1 otherwise. */
int word_list_destroy(struct word_list *wl);

#endif /* WORD_LIST_H */
