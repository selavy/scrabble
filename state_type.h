#ifndef STATE_TYPE_H_
#define STATE_TYPE_H_

#include <stdlib.h>
#include "general.h"
#include "letter_bag_type.h"
#include "letter_frequency.h"
#include "rail_type.h"
#include "move.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

#define BOARD_SIZE 15
#define PLAYER_1 0
#define PLAYER_2 1
#define PLAYER_3 2
#define PLAYER_4 3

//typedef tile_t board_t[BOARD_SIZE][BOARD_SIZE];
typedef tile_t ** board_t;

struct state_t {
  board_t board;
  board_t special_letters;
  int num_players;
  score_t scores[MAX_PLAYERS];
  turn_t turn;
  struct letter_bag_t * letter_bag;
  //  rail_t rails[MAX_PLAYERS];
  rail_t * rails;
  int moves_played;7
};

extern int state_create(struct state_t * state, int players);
extern int state_destroy(struct state_t * state);
extern void state_debug_print_all(struct state_t * state);
extern void state_print(struct state_t * state);
extern int state_place_tile(struct state_t * state, tile_t tile, int row, int col);
extern int state_play_move(struct state_t * state, struct move_t * move);
extern int state_game_over(struct state_t * state);
extern void state_print_winner(struct state_t * state);
extern int state_switch_to_next_player(struct state_t * state);

#endif
