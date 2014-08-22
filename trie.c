#include "trie.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "general.h"
#define LETTERS_IN_ALPHABET 26
#define LINKS LETTERS_IN_ALPHABET
#define char_to_index(c) (c - 'A')

struct trie_t {
  struct trie_t* links[LETTERS_IN_ALPHABET];
};

void to_uppercase(char * str);
void trie_helper_insert(struct trie_t * trie, char * word, int pos);

TRIE_T trie_create(void) {
  struct trie_t * trie = malloc(sizeof(*trie));
  if (!trie) {
    return 0;
  }
  memset(&(trie->links[0]), 0, sizeof(struct trie_t *) * LINKS);
  return trie;
}

int trie_insert(TRIE_T trie, char * word) {
  to_uppercase(word);

  trie_helper_insert(trie, word, 0);
  return SUCCESS;
}

int trie_search(TRIE_T trie, char * word) {
  return SUCCESS;
}

int trie_destroy(TRIE_T trie) {
  int i = 0;
  if(trie) {
    for (; i < LINKS; ++i) {
      trie_destroy(((struct trie_t*) trie)->links[i]);
    }
    free (trie);
    trie = 0;
  }
  return SUCCESS;
}

void to_uppercase(char * str) {
  int i = 0;
  int c;
  for(; str[i] != '\0'; ++i) {
    c = str[i];
    str[i] = toupper(c);
  }
}

int trie_helper_insert(struct trie_t * trie, char * word, int pos) {
  char c;
  if (!word[pos]) { break; }
  c = word[pos];
  if (!trie->links[char_to_index(c)]) {
    trie->links[char_to_index(c)] = malloc(sizeof(struct trie_t));
    if (!trie->links[char_to_index(c)]) {
      return FAILURE;
    }
  }
  return trie_helper_insert(trie->links[char_to_index(c)], word, pos + 1);
}

