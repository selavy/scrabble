#include "tile_drawer.h"

int draw_tiles(struct state_t * state) {
  int i = 0;
  rail_t * rail = &(state->rails[state->turn]);
  tile_t tile;
  for (; i < TILES_ON_RAIL; ++i) {
    if ((*rail)[i] == EMPTY) {
      if ((tile = letter_bag_draw_letter(state->letter_bag)) != ERROR) {
	(*rail)[i] = tile;
      } else {
	(*rail)[i] = EMPTY;
      }
    }
  }
  return SUCCESS;
}
