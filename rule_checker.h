#ifndef RULE_CHECKER_H_
#define RULE_CHECKER_H_

#include "general.h"
#include "state_type.h"
#include "move.h"

extern int rule_checker_create(char * filename);
extern int rule_checker_check_state(struct state_t * state, struct move_t * move);
extern int rule_checker_destroy();
extern int rule_checker_check_word(char * word);

#endif /* end RULE_CHECKER_H_ */
