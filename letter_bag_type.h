#ifndef BAG_TYPE_H_
#define BAG_TYPE_H_

#include "general.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

#define MAX_LETTERS 100

struct letter_bag_t {
  tile_t bag[MAX_LETTERS];
  unsigned char letters;
};

#define letter_bag_is_empty(bag) (!!(bag)->letters)
extern void letter_bag_create(struct letter_bag_t * letter_bag, frequency_t freq);
extern tile_t letter_bag_draw_letter(struct letter_bag_t * letter_bag);
extern void letter_bag_destroy(struct letter_bag_t * letter_bag);
extern void letter_bag_print(struct letter_bag_t * letter_bag);

#endif
