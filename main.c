#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "input.h"
#include "scorer.h"
#include "rule_checker.h"
#include "tile_drawer.h"

//void quit();
void print_error_and_quit(char * error);

int main(int argc, char **argv) {
  struct state_t * state;
  struct move_t * move;
  struct timeval start;
  struct timeval end;
  long long elapsed = 0;
  int ret;
  int tries;
  int skip_move;

  ////////////////   SET UP GAME  //////////////////

  tile_t atile;
  atile = 'A';
  MAKE_BLANK(atile);
  if (IS_BLANK(atile)) {
    printf("blank\n");
  } else {
    printf("not blank\n");
  }
  printf("letter in atile = %c\n", GET_LETTER(atile));

  atile = 'B';
  if (IS_BLANK(atile)) {
    printf("blank\n");
  } else {
    printf("not blank\n");
  }
  printf("letter in atile = %c\n", GET_LETTER(atile));

  exit(0);

  // Open dictionary
  if (argc > 1) {
    gettimeofday(&start, NULL);
    ret = rule_checker_create(argv[1]);
    gettimeofday(&end, NULL);
    printf("Opening dictionary: %s....\n", argv[1]);
  } else {
    gettimeofday(&start, NULL);
    ret = rule_checker_create("en_US.dic");
    gettimeofday(&end, NULL);
    printf("Opening dictionary: %s....\n", "en_US.dic");
  }
  if (ret != SUCCESS) {
    print_error_and_quit("Cannot open dictionary");
  } else {
    elapsed = (end.tv_sec - start.tv_sec)*1000000LL + end.tv_usec - start.tv_usec;
    printf("Sucessfully opened dictionary in %lld microseconds\n\n", elapsed);
  }

  // Get number of players
  ret = get_number_of_players();
  if (ret == QUIT) { goto quit; }
  
  // create the initial game state
  state = malloc(sizeof(*state));
  if (!state) { print_error_and_quit("Unable to allocate memory for game state"); }
  if (state_create(state, ret) != SUCCESS) { print_error_and_quit("Unable to create the game state"); }

  move = malloc(sizeof(*move));
  if (!move) { free(state); print_error_and_quit("Unable to allocate memory for move"); }

  ///////////////    GAME LOOP   /////////////////

  do {
    // get move
    tries = 0;
    skip_move = 0;
    do {
      tries++;
      
      state_print(state);
      ret = get_move(move, state);
      if (ret == QUIT) {
	goto quit;
      } else if (ret != SUCCESS) {
	print_error_and_quit("Unrecognized input from get_move");
      }
      
      if (tries > 10) {
	print_error_and_quit("Too many invalid moves");
      }

      // TODO
      // Check that the move is connected to some other piece of the board
      ret = rule_checker_check_state(state, move);
      if (ret == SUCCESS) {
	skip_move = 0;
	break;
      } else {
	printf("Invalid move!\n");
	skip_move = 1;
	break;
      }
      // check the move
    } while (tries <= 10);

    printf("SKIP MOVE = %d\n", skip_move);

    if (!skip_move) {
      // TODO
      // score the move
      if(score_state(state, move) != SUCCESS) { print_error_and_quit("scoring state"); }
      if(state_play_move(state, move) != SUCCESS) { print_error_and_quit("playing move"); }
      if(draw_tiles(state) != SUCCESS) { print_error_and_quit("drawing tile"); }
    }
    if(state_switch_to_next_player(state) != SUCCESS) { print_error_and_quit("switching players"); }
    
  } while(state_game_over(state) == FALSE);

  state_print(state);
  calculate_final_scores(state);
  state_print_winner(state);

  ///////////////    CLEAN UP   /////////////////

 quit:
  state_destroy(state);
  free(state);
  free(move);
  rule_checker_destroy();
  return 0;
}  

//void quit() {
//  printf("\n\nBye!\n");
//  exit(0);
//}

void print_error_and_quit(char * error) {
  printf("\nError: %s\n", error);
  exit(0);
}
