#include "scorer.h"
#include "string.h"

#define MAX_LIST_SIZE 128
#define MAX_NEW_LIST_SIZE 16

int letter_scores[27] = { 
  1, // A
  3, // B
  3, // C
  2, // D
  1, // E
  4, // F
  2, // G
  4, // H
  1, // I
  8, // J
  5, // K
  1, // L
  3, // M
  1, // N
  1, // O
  3, // P
  10, // Q
  1, // R
  1, // S
  1, // T
  1, // U
  4, // V
  4, // W
  8, // X
  4, // Y
  10, // Z
  0 // BLANK
};

int list_has_word(char * word, char (*list)[BOARD_SIZE], short size);
int is_new_letter(struct move_t * move, int row, int col);

int score_state(struct state_t * state, struct move_t * move) {
  char words[MAX_LIST_SIZE][BOARD_SIZE];
  short words_index = 0;
  int multipliers[MAX_LIST_SIZE][BOARD_SIZE];
  int curr_mults[BOARD_SIZE];
  char new_words[MAX_NEW_LIST_SIZE][BOARD_SIZE];
  short new_words_index = 0;
  short i;
  short j;
  char word[BOARD_SIZE];
  short pos = 0;
  short in_word = 0;
  char c;
  int retVal = SUCCESS;
  int num_moves = move->n;
  int score = 0;
  int word_score = 0;
  int letter_score = 0;
  int word_multiplier = 1;

  // 1. find all words in the original state
  for (i = 0; i < BOARD_SIZE; ++i) {
    for (j = 0; j < BOARD_SIZE; ++j) {
      if (in_word == TRUE) {
	if (state->board[i][j] == EMPTY) {
	  if (pos > 1) {
	    word[pos++] = '\0';
	    strncpy(&(words[words_index++][0]), word, pos);
	  }
	  in_word = FALSE;
	  pos = 0;
	} else {
	  c = state->board[i][j];
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	}
      } else { // not in_word
	if (state->board[i][j] != EMPTY) {
	  pos = 0;
	  c = state->board[i][j];
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	  in_word = TRUE;
	}
      }
    }
    // go to next row
    pos = 0;
    in_word = FALSE;
  }

  pos = 0;
  for (j = 0; j < BOARD_SIZE; ++j) {
    for (i = 0; i < BOARD_SIZE; ++i) {
      if (in_word == TRUE) {
	if (state->board[i][j] == EMPTY) {
	  if (pos > 1) {
	    word[pos++] = '\0';
	    strncpy(&(words[words_index++][0]), word, pos);
	  }
	  in_word = FALSE;
	  pos = 0;
	} else {
	  c = state->board[i][j];
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	}
      } else { // not in_word
	if (state->board[i][j] != EMPTY) {
	  pos = 0;
	  c = state->board[i][j];
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	  in_word = TRUE;
	}
      }
    }
    // go to next row
    pos = 0;
    in_word = FALSE;
  }

#ifdef _DEBUG
  printf("WORDS: \n");
  for (i = 0; i < words_index; ++i) {
    printf("%s\n", words[i]);
  }
#endif

  // 2. add move to board
  for (i = 0; i < num_moves; ++i) {
    state->board[ROW_TO_BOARD(move->placements[i].row)][COL_TO_BOARD(move->placements[i].col)] = move->placements[i].tile;
  }

  // 3. find all words again
  in_word = FALSE;
  pos = 0;
  for (i = 0; i < BOARD_SIZE; ++i) {
    for (j = 0; j < BOARD_SIZE; ++j) {
      if (in_word == TRUE) {
	if (state->board[i][j] == EMPTY) {
	  if (pos > 1) {
	    word[pos++] = '\0';
	    if (list_has_word(word, &(words[0]), words_index) == FAILURE) {
	      memcpy(&(multipliers[new_words_index][0]), curr_mults, sizeof(int) * pos);
	      strncpy(&(new_words[new_words_index++][0]), word, pos);
	    }
	  }
	  in_word = FALSE;
	  pos = 0;
	} else {
	  c = state->board[i][j];
	  curr_mults[pos] = (is_new_letter(move,i,j) == TRUE) ? state->special_letters[i][j] : EMPTY;
	  if (IS_BLANK(c)) { curr_mults[pos] |= BLANK_BIT; }
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	}
      } else { // not in_word
	if (state->board[i][j] != EMPTY) {
	  pos = 0;
	  c = state->board[i][j];
	  curr_mults[pos] = (is_new_letter(move,i,j) == TRUE) ? state->special_letters[i][j] : EMPTY;
	  if (IS_BLANK(c)) { curr_mults[pos] |= BLANK_BIT; }
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	  in_word = TRUE;
	}
      }
    }
    // go to next row
    pos = 0;
    in_word = FALSE;
  }

  in_word = FALSE;
  pos = 0;
  for (j = 0; j < BOARD_SIZE; ++j) {
    for (i = 0; i < BOARD_SIZE; ++i) {
      if (in_word == TRUE) {
	if (state->board[i][j] == EMPTY) {
	  if (pos > 1) {
	    word[pos++] = '\0';
	    if (list_has_word(word, &(words[0]), words_index) == FAILURE) {
	      memcpy(&(multipliers[new_words_index][0]), curr_mults, sizeof(int) * pos);
	      strncpy(&(new_words[new_words_index++][0]), word, pos);
	    }
	  }
	  in_word = FALSE;
	  pos = 0;
	} else {
	  c = state->board[i][j];
	  curr_mults[pos] = (is_new_letter(move,i,j) == TRUE) ? state->special_letters[i][j] : EMPTY;
	  if (IS_BLANK(c)) { curr_mults[pos] |= BLANK_BIT; }
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	}
      } else { // not in_word
	if (state->board[i][j] != EMPTY) {
	  pos = 0;
	  c = state->board[i][j];
	  curr_mults[pos] = (is_new_letter(move,i,j) == TRUE) ? state->special_letters[i][j] : EMPTY;
	  if (IS_BLANK(c)) { curr_mults[pos] |= BLANK_BIT; }
	  word[pos++] = IS_BLANK(c) ? TO_CHAR(GET_LETTER(c)) : TO_CHAR(c);
	  in_word = TRUE;
	}
      }
    }
    // go to next row
    pos = 0;
    in_word = FALSE;
  }

  // 4. score words added
#ifdef _DEBUG
  printf("NEW WORDS: \n");
#endif
  for (i = 0; i < new_words_index; ++i) {
    word_score = 0;
    word_multiplier = 1;
    for (j = 0, c = new_words[i][j]; j < BOARD_SIZE && c != '\0'; ++j, c = new_words[i][j]) {
      letter_score = 0;
      letter_score = letter_scores[TO_LETTER_INDEX(c)];
      if (IS_BLANK(multipliers[i][j])) {
	letter_score = 0;
	multipliers[i][j] &= ~BLANK_BIT;
      }
      if (multipliers[i][j] == DOUBLE_LETTER) {
	letter_score *= 2;
      } else if (multipliers[i][j] == TRIPLE_LETTER) {
	letter_score *= 3;
      } else if (multipliers[i][j] == DOUBLE_WORD) {
	word_multiplier *= 2;
      } else if (multipliers[i][j] == TRIPLE_WORD) {
	word_multiplier *= 3;
      } else if (multipliers[i][j] == MIDDLE_SQUARE) {
	word_multiplier *= 2;
      }
      word_score += letter_score;
    }
    word_score *= word_multiplier;
#ifdef _DEBUG
    printf("\n%s --> %d\n", new_words[i], word_score);
#endif
    score += word_score;
  }
  state->scores[state->turn] += score;
  if (move->n == TILES_ON_RAIL) { state->scores[state->turn] += BINGO_VALUE; }

  // remove the tiles
  for (i = 0; i < num_moves; ++i) {
    state->board[ROW_TO_BOARD(move->placements[i].row)][COL_TO_BOARD(move->placements[i].col)] = EMPTY;
  }
  
  return retVal;
}

int calculate_final_scores(struct state_t * state) {
  return SUCCESS;
}

int list_has_word(char * word, char (*list)[BOARD_SIZE], short size) {
  short i = 0;
  for (; i < size; ++i) {
    if (strcmp(word, &(list[i][0])) == 0) return SUCCESS;
  }
  return FAILURE;
}

int is_new_letter(struct move_t * move, int row, int col) {
  int num_moves = move->n;
  int i = 0;
  
  for(; i < num_moves; ++i) {
    if (ROW_TO_BOARD(move->placements[i].row) == row && COL_TO_BOARD(move->placements[i].col) == col) {
      return TRUE;
    }
  }
  return FALSE;
}
