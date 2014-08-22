#include "rule_checker.h"

#define IS_WORD 2
#define NOT_WORD -2

static int initialized = FALSE;

int rule_checker_load_dictionary(char * filename);
int rule_checker_check_word(char * word);

int rule_checker_create(char * filename) {
  if (initialized) {
    return SUCCESS;
  } else { // open the dictionary
  //  return FAILURE;
    return SUCCESS;
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
    return SUCCESS;
  } else {
    initialized = TRUE;
    return SUCCESS;
  }
}

int rule_checker_load_dictionary(char * filename) {
  return SUCCESS;
}

int rule_checker_check_word(char * word) {
  return IS_WORD;
}


