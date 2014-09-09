#include "rule_checker.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HORZ 1
#define VERT 2

static TRIE_T trie = 0;
static int initialized = FALSE;

int rule_checker_load_dictionary(char * filename);
int check_if_straight_line_move(struct move_t * move);
int check_if_adjacent_tiles(board_t * board, struct move_t * move, int direction);
void print_reason(char * str);
int check_if_played_first_square(struct move_t * move);
int check_if_connected_to_existing_word(board_t * board, struct move_t * move, int direction);
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

// TODO
// Check that the move played is connected to some other piece of the board
int rule_checker_check_state(struct state_t * state, struct move_t * move) {
  int i = 0;
  int j = 0;
  int num_moves = move->n;
  int row;
  int col;
  tile_t tile;
  int in_word = 0;
  char word[BOARD_SIZE + 1];
  int pos = 0;
  int retVal = SUCCESS;
  char c;
  int result;

  // whether or not it is an official rule, i want to force the first player to play on the
  // middle square
  if (state->moves_played == 0 && check_if_played_first_square(move) == FAILURE) {
    print_reason("first move was not played on the middle square");
    return FAILURE;
  }

  if ((result = check_if_straight_line_move(move)) == FAILURE) {
    print_reason("move placed is not a straight line");
    return FAILURE;
  }

  // Check if the have tile and the square isn't already occupied
  for (i = 0; i < num_moves; ++i) {
    row = move->placements[i].row;
    col = move->placements[i].col;
    tile = move->placements[i].tile;
    if (rail_find_tile(state->rails[state->turn], tile) == TILE_NOT_FOUND) {
      printf("Do not have tile: %c\n", TILE_TO_CHAR(tile));
      print_reason("tried to play tile that you do not have");
      return FAILURE;
    }
    if (state->board[ROW_TO_BOARD(row)][COL_TO_BOARD(col)] != EMPTY) {
      printf("Space (%d,%d) is occupied\n", row, col);
      print_reason("tried to play on occupied square");
      return FAILURE;
    }
  }

  if (check_if_connected_to_existing_word(&state->board, move, result) == FAILURE) {
    print_reason("word is not connected to any existing words");
    return FAILURE;
  }

  // place tiles just for the checking
  // if a check fails, must set retVal = FAILURE and goto failure
  for (i = 0; i < num_moves; ++i) {
    state->board[ROW_TO_BOARD(move->placements[i].row)][COL_TO_BOARD(move->placements[i].col)] = move->placements[i].tile;
  }

  if (check_if_adjacent_tiles(&state->board, move, result) == FAILURE) {
    print_reason("letters are not all adjacent");
    retVal = FAILURE;
    goto failure;
  }

  for (i = 0; i < BOARD_SIZE; ++i) {
    for (j = 0; j < BOARD_SIZE; ++j) {
      if (in_word) {
	if (state->board[i][j] >= EMPTY) {
	  if (pos > 1) {
	    word[pos] = '\0';
	    if (trie_search(trie, word) != SUCCESS) {
	      retVal = FAILURE;
	      goto failure;
	    }
	  }
	  in_word = 0;
	  pos = 0;
	} else {
	  c = state->board[i][j];
	  if (!IS_BLANK(c)) {
	    c = TO_CHAR(c);
	  }
	  word[pos++] = c;
	}
      } else { // not checking word
	if (state->board[i][j] != EMPTY) {
	  pos = 0;
	  c = state->board[i][j];
	  if (!IS_BLANK(c)) {
	    c = TO_CHAR(c);
	  }
	  word[pos++] = c;
	  in_word = 1;
	}
      }
    }

    if (in_word) {
      if (pos > 1) {
	word[pos] = '\0';
	if (trie_search(trie, word) != SUCCESS) {
	  retVal = FAILURE;
	  goto failure;
	}
      }
    }

    pos = 0;
    in_word = 0;
  }

  pos = 0;
  in_word = 0;
  for (j = 0; j < BOARD_SIZE; ++j) {
    for (i = 0; i < BOARD_SIZE; ++i) {
      if (in_word) {
	if (state->board[i][j] >= EMPTY) {
	  if (pos > 1) {
	    word[pos] = '\0';
	    if (trie_search(trie, word) != SUCCESS) {
	      retVal = FAILURE;
	      goto failure;
	    }
	  }
	  in_word = 0;
	  pos = 0;
	} else {
	  c = state->board[i][j];
	  if (!IS_BLANK(c)) {
	    c = TO_CHAR(c);
	  }
	  word[pos++] = c;
	}
      } else { // not checking word
	if (state->board[i][j] != EMPTY) {
	  pos = 0;
	  c = state->board[i][j];
	  if (!IS_BLANK(c)) {
	    c = TO_CHAR(c);
	  }
	  word[pos++] = c;
	  in_word = 1;
	}
      }
    }

    if (in_word) {
      if (pos > 1) {
	word[pos] = '\0';
	if (trie_search(trie, word) != SUCCESS) {
	  retVal = FAILURE;
	  goto failure;
	}
      }
    }
    pos = 0;
    in_word = 0;
  }


 failure:
  // remove the tiles
  for (i = 0; i < num_moves; ++i) {
    state->board[ROW_TO_BOARD(move->placements[i].row)][COL_TO_BOARD(move->placements[i].col)] = EMPTY;
  }
  
  return retVal;
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
  if(!filename) { return FAILURE; }
  file = fopen(filename, "r");
  if (!file) { return FAILURE; }
  
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
  if (!initialized) { return FAILURE; }
  return trie_search(trie, word);
}

int check_if_straight_line_move(struct move_t * move) {
  int horz_changed = 0;
  int vert_changed = 0;
  int horz = 0;
  int vert = 0;
  int i = 0;
  int num_moves = move->n;
  
  horz = move->placements[0].row;
  vert = move->placements[0].col;
  for (; i < num_moves; ++i) {
    if (move->placements[i].row != horz) {
      horz_changed = 1;
    }
    if (move->placements[i].col != vert) {
      vert_changed = 1;
    }
    if (horz_changed && vert_changed) {
      return FAILURE;
    }
  }

  return horz_changed ? VERT : HORZ;
}

void print_reason(char * str) {
  #ifdef _DEBUG
  printf("Rule Checker Failed: %s\n", str);
  #endif
}

int check_if_adjacent_tiles(board_t * board, struct move_t * move, int direction) {
  int i;
  int num_moves = move->n;
  int min = BOARD_SIZE;
  int max = -1;
  int other = 0;

  if (direction == HORZ) {
    for (i = 0; i < num_moves; ++i) {
      if (move->placements[i].col < min) {
	min = move->placements[i].col;
      }
      if (move->placements[i].col > max) {
	max = move->placements[i].col;
      }
    }
    other = ROW_TO_BOARD(move->placements[0].row);
    for (i = min; i < max; ++i) {
      if ((*board)[other][COL_TO_BOARD(i)] == EMPTY) {
	return FAILURE;
      }
    }
  } else if (direction == VERT) {
    for (i = 0; i < num_moves; ++i) {
      if (move->placements[i].row < min) {
	min = move->placements[i].row;
      }
      if (move->placements[i].row > max) {
	max = move->placements[i].row;
      }
    }
    other = COL_TO_BOARD(move->placements[0].col);
    for (i = min; i < max; ++i) {
      if ((*board)[ROW_TO_BOARD(i)][other] == EMPTY) {
	return FAILURE;
      }
    }
  } else {
    return FAILURE;
  }
  return SUCCESS;
}

int check_if_played_first_square(struct move_t * move) {
  int i = 0;
  int num_moves = move->n;
  for (; i < num_moves; ++i) {
    if (move->placements[i].row == MIDDLE_ROW && move->placements[i].col == MIDDLE_COL) {
      return SUCCESS;
    }
  }
  return FAILURE;
}

int check_if_connected_to_existing_word(board_t * board, struct move_t * move, int direction) {
  int i = 0;
  int num_moves = move->n;
  int row;
  int col;

  if ((*board)[BOARD_SIZE/2][BOARD_SIZE/2] == EMPTY) { return SUCCESS; }

  for (i = 0; i < num_moves; ++i) {
    row = ROW_TO_BOARD(move->placements[i].row);
    col = COL_TO_BOARD(move->placements[i].col);
    if (row+1 < BOARD_SIZE && (*board)[row+1][col] != EMPTY) { return SUCCESS; }
    if (row > 1              && (*board)[row-1][col] != EMPTY) { return SUCCESS; }
    if (col + 1 < BOARD_SIZE && (*board)[row][col+1] != EMPTY) { return SUCCESS; }
    if (col > 1              && (*board)[row][col-1] != EMPTY) { return SUCCESS; }
  }
  return FAILURE;
}
