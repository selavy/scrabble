#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "trie.h"
#include "general.h"
#include "rule_checker.h"

const char * WORD = "word";
const char * ANOTHER = "another";
const char * WORDY = "wordy";

void find_word(TRIE_T trie, char * word) {
  printf("Finding: %s...\t", word);
  if (trie_search(trie, word) == SUCCESS) {
    printf("Found!\n");
  } else {
    printf("Not Found!\n");
  }
}

void find_rule_checker_word(char * word) {
  char * w = malloc(strlen(word) + 1);
  if (!w) { printf("ERROR creating word: %s\n", word); return; }
  strcpy(w, word);
  printf("Finding: %s...\t", w);
  if (rule_checker_check_word(w) == SUCCESS) {
    printf("Found!\n");
  } else {
    printf("Not Found!\n");
  }
  if(w) { free(w); }
}

char * create_word(TRIE_T trie, const char * word) {
  char * w = malloc(strlen(word) + 1);
  if (!w) { printf("ERROR inserting: %s", word); return 0; }
  strcpy(w, word);
  return w;
}

int main(int argc, char ** argv) {
  struct timeval start;
  struct timeval end;
  long long elapsed = 0;
  TRIE_T trie = trie_create();
  char * word = create_word(trie, WORD);
  char * another = create_word(trie, ANOTHER);
  char * wordy = create_word(trie, WORDY);

  printf("BASIC TRIE TEST\n");
  printf("inserting: 'word', 'wordy'\n");
  gettimeofday(&start, NULL);
  trie_insert(trie, word);
  trie_insert(trie, wordy);
  find_word(trie, word);
  find_word(trie, another);
  find_word(trie, wordy);
  trie_destroy(trie);
  gettimeofday(&end, NULL);
  elapsed = (end.tv_sec - start.tv_sec)*1000000LL + end.tv_usec - start.tv_usec;
  printf("TEST TOOK %lld microseconds\n", elapsed);

  free(word);
  free(another);
  free(wordy);

  printf("\n\nRULE CHECKER DICTIONARY TEST\n");
  gettimeofday(&start, NULL);
  if (rule_checker_create("dictionary.dat") != SUCCESS) {
    printf("Unable to open dictionary!\n"); goto error;
  }
  find_rule_checker_word("word");
  find_rule_checker_word("peter");
  find_rule_checker_word("king");
  find_rule_checker_word("knight");
  gettimeofday(&end, NULL);
  elapsed = (end.tv_sec - start.tv_sec)*1000000LL + end.tv_usec - start.tv_usec;
  printf("TEST TOOK %lld microseconds\n", elapsed);
 error:
  rule_checker_destroy();

  printf("\nLOADING FULL DICTIONARY\n");
  gettimeofday(&start, NULL);
  if (rule_checker_create("en_US.dic") != SUCCESS) {
    printf("Unable to open dictionary: %s\n", "en_US.dic");
  }
  gettimeofday(&end, NULL);
  elapsed = (end.tv_sec - start.tv_sec)*1000000LL + end.tv_usec - start.tv_usec;
  printf("TEST TOOK %lld microseconds\n", elapsed);
  rule_checker_destroy();
  return 0;
}
