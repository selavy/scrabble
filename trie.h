#ifndef TRIE_H_
#define TRIE_H_

typedef void* TRIE_T;

extern TRIE_T trie_create(void);
extern int trie_insert(TRIE_T trie, char * word);
extern int trie_search(TRIE_T trie, char * word);
extern int trie_destroy(TRIE_T trie);

extern void to_uppercase(char * str);

#endif
