#include "trie.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
// DEBUG
#include <stdio.h>
// END DEBUG
#include "general.h"
#define LETTERS_IN_ALPHABET 26
#define LINKS LETTERS_IN_ALPHABET
#define char_to_index(c) (c - 'A')

struct trie_t {
  struct trie_t* links[LETTERS_IN_ALPHABET];
};

void to_uppercase(char * str);
int trie_helper_insert(struct trie_t * trie, char * word, int pos);
int trie_helper_search(struct trie_t * trie, char * word, int pos);

TRIE_T trie_create(void) {
  //  struct trie_t * trie = malloc(sizeof(*trie));
  struct trie_t * trie = calloc(1, sizeof(*trie));
  if (!trie) {
    return 0;
  }
  //  memset(&(trie->links[0]), 0, sizeof(struct trie_t *) * LINKS);
  return trie;
}

int trie_insert(TRIE_T trie, char * word) {
  if (!trie) return FAILURE;
  to_uppercase(word);
  trie_helper_insert(trie, word, 0);
  return SUCCESS;
}

int trie_search(TRIE_T trie, char * word) {
  if (!trie) return FAILURE;
  if (!word) return FAILURE;
  if (strlen(word) == 0) return FAILURE;
  to_uppercase(word);
  return trie_helper_search(trie, word, 0);
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
    if (str[i] != '*') {
      c = str[i];
      str[i] = toupper(c);
    }
  }
}

int trie_helper_insert(struct trie_t * trie, char * word, int pos) {
  char c;
  if (!word[pos]) { return SUCCESS; }
  c = word[pos];
  if (!trie->links[char_to_index(c)]) {
    //    trie->links[char_to_index(c)] = malloc(sizeof(struct trie_t));
    trie->links[char_to_index(c)] = calloc(1, sizeof(struct trie_t));
    //    memset(&(trie->links[char_to_index(c)][0]), 0, sizeof(struct trie_t *) * LINKS);
    if (!trie->links[char_to_index(c)]) {
      return FAILURE;
    }
  }
  return trie_helper_insert(trie->links[char_to_index(c)], word, pos + 1);
}

int trie_helper_search(struct trie_t * trie, char * word, int pos) {
  tile_t blank_letter;
  if (!word[pos] || word[pos] == '\0' ) { return SUCCESS; }
  else if (IS_BLANK(word[pos])) {
    //    int i = 0;
    //    for (; i < 26; ++i) {
    //      if (trie_helper_search(trie->links[i], word, pos + 1) == SUCCESS) {
    //	printf("Blank matched with letter: %c\n", i + 'A');
    //	return SUCCESS;
    //      }
    //    }
    //    return FAILURE;
    blank_letter = GET_LETTER(word[pos]);
    printf("DEBUG: blank letter = %d\n", blank_letter);
    if (!trie->links[blank_letter]) { return FAILURE; }
    else { return trie_helper_search(trie->links[blank_letter], word, pos + 1); }
  }
  else if (word[pos] < 'A' || word[pos] > 'Z') { printf("can't play: %d\n", (int)word[pos]); return FAILURE; }
  else if (!trie->links[char_to_index(word[pos])]) {  return FAILURE; }
  else { return trie_helper_search(trie->links[char_to_index(word[pos])], word, pos + 1); }
}
