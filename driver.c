#include <stdio.h>
#include <stdlib.h>
#include "trie.h"

int main(int argc, char ** argv) {
  TRIE_T trie = trie_create();
  trie_destroy(trie);
  return 0;
}
