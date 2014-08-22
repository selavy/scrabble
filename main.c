#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "scorer.h"
#include "rule_checker.h"
#include "tile_drawer.h"

//void quit();
void print_error_and_quit(char * error);

int main(int argc, char **argv) {
  struct state_t * state;
  struct move_t * move;
  int ret;
  int tries;

  ////////////////   SET UP GAME  //////////////////

  // Open dictionary
  if (argc > 1) {
    ret = rule_checker_create(argv[1]);
  } else {
    ret = rule_checker_create("dictionary.dict");
  }
  if (ret != SUCCESS) {
    print_error_and_quit("Cannot open dictionary");
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
    do {
      tries++;
      
      state_print(state);
      ret = get_move(move, state);
      if (ret == QUIT) {
	//	quit();
	goto quit;
      } else if (ret != SUCCESS) {
	print_error_and_quit("Unrecognized input from get_move");
      }
      
      if (tries > 10) {
	print_error_and_quit("Too many invalid moves");
      }
      
      // check the move
    } while (rule_checker_check_state(state, move) != SUCCESS);

    if(score_state(state, move) != SUCCESS) { print_error_and_quit("scoring state"); }
    if(state_play_move(state, move) != SUCCESS) { print_error_and_quit("playing move"); }
    if(draw_tiles(state) != SUCCESS) { print_error_and_quit("drawing tile"); }
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
