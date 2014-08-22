#ifndef SCORER_H_
#define SCORER_H_

#include "state_type.h"
#include "move.h"

extern int score_state(struct state_t * state, struct move_t * move);
extern int calculate_final_scores(struct state_t * state);

#endif /* SCORER_H_ */
