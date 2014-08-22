#ifndef INPUT_MODULE_H_
#define INPUT_MODULE_H_

#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include "state_type.h"
#include "move.h"

#define SQUARE_FULL -4
#define INVALID_MOVE -5
#define QUIT -6
#define KEEP_PLAYING 2
#define RETRIES 5

struct state_t;

extern int get_number_of_players();
extern int get_move(struct move_t * move, struct state_t * state);

#endif
