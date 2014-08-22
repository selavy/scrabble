#include "rule_checker.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_WORD 2
#define NOT_WORD -2

static TRIE_T trie = 0;

static int initialized = FALSE;

int rule_checker_load_dictionary(char * filename);
//int rule_checker_check_word(char * word);

int rule_checker_create(char * filename) {
  int retVal = FAILURE;
  if (initialized) {
    return SUCCESS;
  } else { // open the dictionary
    trie = trie_create();
    if (!trie) {
      return FAILURE;
    } else {
      retVal = rule_checker_load_dictionary(filename);
      if (retVal == SUCCESS) {
	initialized = TRUE;
      } else {
	initialized = FALSE;
      }
      return retVal;
    }
  }
}

int rule_checker_check_state(struct state_t * state, struct move_t * move) {
  int i = 0;
  int num_moves = move->n;
  int row;
  int col;
  tile_t tile;

  // Check if the have tile and the square isn't already occupied
  for (; i < num_moves; ++i) {
    row = move->placements[i].row;
    col = move->placements[i].col;
    tile = move->placements[i].tile;
    if (rail_find_tile(state->rails[state->turn], tile) == TILE_NOT_FOUND) {
      printf("Do not have tile: %c\n", tile);
      return FAILURE;
    }
    if (state->board[ROW_TO_BOARD(row)][COL_TO_BOARD(col)] != EMPTY) {
      printf("Space (%d,%d) is occupied\n", row, col);
      return FAILURE;
    }
  }
  
  return SUCCESS;
}

int rule_checker_destroy() {
  if (initialized) {
    initialized = FALSE;
    trie_destroy(trie);
    trie = 0;
    return SUCCESS;
  } else {
    return FAILURE;
  }
}

int rule_checker_load_dictionary(char * filename) {
  FILE * file = 0;
  char * line = 0;
  char * word = 0;
  size_t len = 0;
  ssize_t read = 0;
  if(!filename) return FAILURE;
  file = fopen(filename, "r");
  if (!file) return FAILURE;
  
  while ((read = getline(&line, &len, file)) > 0) {
    word = malloc(read + 1);
    if (!word) { goto error; }
    if (sscanf(line, "%s\n", word) != 1) {
      printf("Dictionary not formatted correctly!\n"); goto error;
    } else {
      trie_insert(trie, word);
    }
    if (word) { free(word);  word = 0; }
    if (line) { free(line);  line = 0; }
    len = 0;
  }

  if (line) { free(line); }
  if (file) { fclose(file); }
  if (word) { free(word); }
  return SUCCESS;
  
 error:
  if (line) { free(line); }
  fclose(file);
  return FAILURE;
}

int rule_checker_check_word(char * word) {
  if (!initialized) return FAILURE;
  return trie_search(trie, word);
}
