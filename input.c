#include "input.h"

int user_wants_to_quit();
int valid_move(struct move_t * move);
int get_input(struct move_t * move);

///////////////////////////// MODULE INTERFACE FUNCTIONS ////////////////////////////////////

int get_number_of_players() {
  char * line = 0;
  size_t len = 0;
  ssize_t read;
  int num_players = 2;
  int tries = 0;

  while(tries < 5) {
    tries++;
    printf("How many players? [2-4] (default = 2): ");
    if ((read = getline(&line, &len, stdin)) != -1) {
      if (line[0] == '\n') {
	break;
      } else if(sscanf(line, "%d\n", &num_players) == 1) {
	if (num_players <= 4 && num_players > 0) {
	  break;
	} else {
	  num_players = 2;
	  continue;
	}
      }
    }
    if (user_wants_to_quit() == QUIT) {
      num_players = QUIT;
    }
  }
  if (line) {
    free(line);
  }
  return num_players;
}

int get_move(struct move_t * move, struct state_t * state) {
  int ret;
  int i = 0;
  for (; i < RETRIES; ++i) {
    printf("Move? (Blank line to finish): ");
    ret = get_input(move);
    if (ret == QUIT) {
      return QUIT;
    }
    else if (ret != SUCCESS) {
      if (user_wants_to_quit() == QUIT) {
	return QUIT;
      }
    } else if (ret == SUCCESS) {
      if (valid_move(move) == TRUE) {
	return SUCCESS;
      } else {
	printf("Invalid move\n");
      }
    }
  }
  return FAILURE;
}

////////////////////////////// HELPER FUNCTIONS /////////////////////////////////////////////

int user_wants_to_quit() {
  char * input = 0;
  size_t len = 0;
  ssize_t read = 0;
  int retVal = QUIT;

  printf("Quit? ");
  if ((read = getline(&input, &len, stdin)) != -1) {
    if (input[0] == 'Y' || input[0] == 'y') {
      retVal = QUIT;
    } else {
      retVal = KEEP_PLAYING;
    }
  }
  if (input) {
    free(input);
  }
  return retVal;
}

int valid_move(struct move_t * move) {
  int i = 0;
  int moves = move->n;
  int row;
  int col;
  for (; i < moves; ++i) {
    row = move->placements[i].row;
    col = move->placements[i].col;
    if (row < 1 || row > BOARD_SIZE || col < 1 || col > BOARD_SIZE) {
      return FALSE;
    }
  }
  return TRUE;
}

int get_input(struct move_t * move) {
  char * input = 0;
  size_t len = 0;
  ssize_t read = 0;
  int row = 0;
  int col = 0;
  char letter = 0;
  int i = 0;
  memset(&(move->placements[0]), 0, sizeof(struct placement_t) * MAX_PLACEMENTS);
  for (; i < TILES_ON_RAIL; ++i ) {
    if ((read = getline(&input, &len, stdin)) != -1) {
      if (input[0] == '\n') {
	free(input);
	break;
      } else if (len > 6 && strncmp(input, "quit\n", 6) == 0) {
	free(input);
	return QUIT;
      } else if (sscanf(input, "%d %d %c\n", &row, &col, &letter) == 3) {
	if (letter >= 'a') {
	  letter -= 'a' - 'A';
	}
	move->placements[i].row = row;
	move->placements[i].col = col;
	move->placements[i].tile = letter;
	free(input);
	input = 0;
	len = 0;
      }
    } else {
      if (input) {
	free(input);
      }
      return FAILURE;
    }
  }
  move->n = i;
  return (i == 0) ? FAILURE : SUCCESS;
}
