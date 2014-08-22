#include "state_type.h"

static void print_board(board_t board, board_t special_letters);
static void print_turn(turn_t turn);
static int distribute_tiles(struct state_t * state);

int state_create(struct state_t * state, int players) {
  int i;
  state->moves_played = 0;
  state->num_players = players;
  state->turn = PLAYER_1;
  memset(&(state->scores[0]), 0, sizeof(score_t) * players);

  state->letter_bag = malloc(sizeof(*(state->letter_bag)));
  if (!state->letter_bag) { return FAILURE; }
  letter_bag_create(state->letter_bag, standard_frequency);
  
  state->rails = malloc(sizeof(*(state->rails)) * players);
  if (!state->rails) { return FAILURE; }
  //  rails_create(&(state->rails[0]), players);
  rails_create(state->rails, players);

  if (distribute_tiles(state) != SUCCESS) { return FAILURE; }

  state->board = malloc(sizeof(*(state->board)) * BOARD_SIZE);
  state->special_letters = malloc(sizeof(*(state->special_letters)) * BOARD_SIZE);

  for (i = 0; i < BOARD_SIZE; ++i) {
    state->board[i] = malloc(sizeof(**(state->board)) * BOARD_SIZE);
    state->special_letters[i] = malloc(sizeof(**(state->special_letters)) * BOARD_SIZE);
    memset(state->board[i], EMPTY, sizeof(tile_t) * BOARD_SIZE);
    memset(state->special_letters[i], EMPTY, sizeof(tile_t) * BOARD_SIZE);
  }

  state->special_letters[7][7] = MIDDLE_SQUARE;
  state->special_letters[6][6] = DOUBLE_LETTER;
  state->special_letters[6][8] = DOUBLE_LETTER;
  state->special_letters[8][6] = DOUBLE_LETTER;
  state->special_letters[8][8] = DOUBLE_LETTER;
  state->special_letters[5][5] = TRIPLE_LETTER;
  state->special_letters[5][9] = TRIPLE_LETTER;
  state->special_letters[9][5] = TRIPLE_LETTER;
  state->special_letters[9][9] = TRIPLE_LETTER;
  state->special_letters[4][4] = DOUBLE_WORD;
  state->special_letters[4][10] = DOUBLE_WORD;
  state->special_letters[10][4] = DOUBLE_WORD;
  state->special_letters[10][10] = DOUBLE_WORD;
  state->special_letters[3][3] = DOUBLE_WORD;
  state->special_letters[3][11] = DOUBLE_WORD;
  state->special_letters[11][3] = DOUBLE_WORD;
  state->special_letters[11][11] = DOUBLE_WORD;
  state->special_letters[2][2] = DOUBLE_WORD;
  state->special_letters[2][12] = DOUBLE_WORD;
  state->special_letters[12][2] = DOUBLE_WORD;
  state->special_letters[12][12] = DOUBLE_WORD;
  state->special_letters[1][1] = DOUBLE_WORD;
  state->special_letters[1][13] = DOUBLE_WORD;
  state->special_letters[13][1] = DOUBLE_WORD;
  state->special_letters[13][13] = DOUBLE_WORD;
  state->special_letters[0][0] = TRIPLE_WORD;
  state->special_letters[0][14] = TRIPLE_WORD;
  state->special_letters[14][0] = TRIPLE_WORD;
  state->special_letters[14][14] = TRIPLE_WORD;
  state->special_letters[7][3] = DOUBLE_LETTER;
  state->special_letters[3][7] = DOUBLE_LETTER;
  state->special_letters[11][7] = DOUBLE_LETTER;
  state->special_letters[7][11] = DOUBLE_LETTER;
  state->special_letters[0][7] = TRIPLE_WORD;
  state->special_letters[7][0] = TRIPLE_WORD;
  state->special_letters[14][7] = TRIPLE_WORD;
  state->special_letters[7][14] = TRIPLE_WORD;
  state->special_letters[2][6] = DOUBLE_LETTER;
  state->special_letters[6][2] = DOUBLE_LETTER;
  state->special_letters[2][8] = DOUBLE_LETTER;
  state->special_letters[8][2] = DOUBLE_LETTER;
  state->special_letters[6][12] = DOUBLE_LETTER;
  state->special_letters[12][6] = DOUBLE_LETTER;
  state->special_letters[8][12] = DOUBLE_LETTER;
  state->special_letters[12][8] = DOUBLE_LETTER;
  state->special_letters[5][1] = TRIPLE_LETTER;
  state->special_letters[1][5] = TRIPLE_LETTER;
  state->special_letters[9][1] = TRIPLE_LETTER;
  state->special_letters[1][9] = TRIPLE_LETTER;
  state->special_letters[9][13] = TRIPLE_LETTER;
  state->special_letters[13][9] = TRIPLE_LETTER;
  state->special_letters[5][13] = TRIPLE_LETTER;
  state->special_letters[13][5] = TRIPLE_LETTER;
  state->special_letters[0][3] = DOUBLE_LETTER;
  state->special_letters[3][0] = DOUBLE_LETTER;
  state->special_letters[0][11] = DOUBLE_LETTER;
  state->special_letters[11][0] = DOUBLE_LETTER;
  state->special_letters[14][3] = DOUBLE_LETTER;
  state->special_letters[3][14] = DOUBLE_LETTER;
  state->special_letters[11][14] = DOUBLE_LETTER;
  state->special_letters[14][11] = DOUBLE_LETTER;

  return SUCCESS;
}

int state_destroy(struct state_t * state) {
  int i = 0;
  free(state->letter_bag);
  for (; i < BOARD_SIZE; ++i) {
    free(state->board[i]);
    free(state->special_letters[i]);
  }
  free(state->rails);
  free(state->board);
  free(state->special_letters);
  return SUCCESS;
}

void state_debug_print_all(struct state_t * state) {
  #ifdef _DEBUG
  int i = 0;

  printf("STATE BEGIN\n");
  printf("Number of players: %d\n", state->num_players);
  for (; i < state->num_players; ++i) {
    printf("Player #%d rail: ", i + 1);
    rail_print(state->rails[i]);
    PRT_EOL;
  }
  printf("Board: ");
  print_board(state->board, state->special_letters);
  PRT_EOL;
  printf("Letter bag: ");
  letter_bag_print(state->letter_bag);
  PRT_EOL;
  printf("To Move: ");
  print_turn(state->turn);
  PRT_EOL;
  printf("STATE END\n");
  #endif
}

void state_print(struct state_t * state) {
  state_print_winner(state);
  print_board(state->board, state->special_letters);
  printf("Player #%d to move\n", state->turn);
  rail_print(state->rails[state->turn]);
}

int state_place_tile(struct state_t * state, tile_t tile, int row, int col) {
  if (row > 0 && row <= BOARD_SIZE && col > 0 && col <= BOARD_SIZE) {
    state->board[ROW_TO_BOARD(row)][COL_TO_BOARD(col)] = tile;
    return SUCCESS;
  } else {
    return ERROR;
  }
}

void print_board(board_t board, board_t special_letters) {
  int i = 0;
  int j = 0;
    printf("\t _____ _____ _____ _____ _____ _____ _____ _____ _____ _____ _____ _____ _____ _____ _____\n");
  for (; i < BOARD_SIZE; ++i) {

    printf("\t|     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |");
    PRT_EOL;
    PRT_TAB;
    printf("|");
    for (j = 0; j < BOARD_SIZE; ++j) {
      if (board[i][j] == EMPTY) {
	if (special_letters[i][j] == DOUBLE_LETTER) {
	  printf("  DL |");
	} else if (special_letters[i][j] == TRIPLE_LETTER) {
	  printf("  TL |");
	} else if (special_letters[i][j] == DOUBLE_WORD) {
	  printf("  DW |");
	} else if (special_letters[i][j] == TRIPLE_WORD) {
	  printf("  TW |");
	} else if (special_letters[i][j] == MIDDLE_SQUARE) {
	  printf("  #  |");
	} else {
	  printf("     |");
	}
      } else {
	printf("  %c  |", TILE_TO_CHAR(board[i][j]));
      }
    }
    printf("\n\t|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|\n");
  }
}

void print_turn(turn_t turn) {
  char turn_str[9];
  switch (turn) {
  case PLAYER_1:
    strncpy(&(turn_str[0]), "Player 1", 9);
    break;
  case PLAYER_2:
    strncpy(&(turn_str[0]), "Player 2", 9);
    break;
  case PLAYER_3:
    strncpy(&(turn_str[0]), "Player 3", 9);
    break;
  case PLAYER_4:
    strncpy(&(turn_str[0]), "Player 4", 9);
    break;
  default:
    strncpy(&(turn_str[0]), "Unknown", 9);
  }

  // just in case :)
  turn_str[8] = '\0';
  printf("%s", &(turn_str[0]));
}

int distribute_tiles(struct state_t * state) {
  int rail;
  int letter;
  tile_t tile;
  int num_players = state->num_players;

  for(rail = 0; rail < num_players; ++rail ) {
    for (letter = 0; letter < TILES_ON_RAIL; ++letter) {
      do {
	tile = letter_bag_draw_letter(state->letter_bag);
	if (tile == ERROR) return FAILURE;
      } while(rail_add_tile(state->rails[rail], tile) == FAILURE);
    }
  }
  return SUCCESS;
}

int state_game_over(struct state_t * state) {
  int i;
  int num_players = state->num_players;
  if (letter_bag_is_empty((state->letter_bag))) {
    for (i = 0; i < num_players; ++i) {
      if (rail_empty(state->rails[i]) == TRUE) { return TRUE; }
    }
  }
  return FALSE;
}

void state_print_winner(struct state_t * state) {
  int i = 0;
  int num_players = state->num_players;
  printf("PLAYER\t\tSCORE\n");
  printf("-------------------------------\n");
  for (; i < num_players; ++i) {
    printf("%d\t\t%5lu\n", i + 1, state->scores[i]);
  }
}

int state_play_move(struct state_t * state, struct move_t * move) {
  int i = 0;
  int moves = move->n;
  for (; i < moves; ++i) {
    state_place_tile(state, CHAR_TO_TILE(move->placements[i].tile), move->placements[i].row, move->placements[i].col);
    if (rail_use_tile(state->rails[state->turn], move->placements[i].tile) == TILE_NOT_FOUND) {
      return FAILURE;
    }
  }
  return SUCCESS;
}

int state_switch_to_next_player(struct state_t * state) {
  state->moves_played++;
  state->turn = (state->turn + 1) % state->num_players;
  return SUCCESS;
}
