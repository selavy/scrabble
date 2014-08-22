#include "rail_type.h"

void rails_create(rail_t * rail, int n) {
  int i = 0;
  for (; i < n; ++i) {
    rail_create(rail[i]);
  }
}

void rail_create(rail_t rail) {
  memset(&(rail[0]), EMPTY, TILES_ON_RAIL * sizeof(tile_t));
}

int rail_add_tile(rail_t rail, tile_t tile) {
  int i = 0;
  for (; i < TILES_ON_RAIL; ++i) {
    if (rail[i] == EMPTY) {
      rail[i] = tile;
      return i;
    }
  }
  return FAILURE;
}

void rail_destroy(rail_t rail) {
  // empty
}

void rails_destroy(rail_t * rail, int n) {
  // empty
}

void rail_print(rail_t rail) {
#ifdef _DEBUG
  int i = 0;
  for (; i < TILES_ON_RAIL; ++i) {
    if (rail[i] == EMPTY) {
      printf("_ ");
    } else if (rail[i] == BLANK) {
      printf("* ");
    } else {
      printf("%c ", TILE_TO_CHAR(rail[i]));
    }
  }
  printf("\n");
#endif
}

int rail_find_tile(rail_t rail, tile_t tile) {
  int i = 0;
  for (; i < TILES_ON_RAIL; ++i) {
    if (rail[i] == CHAR_TO_TILE(tile)) {
      return i;
    }
  }
  return TILE_NOT_FOUND;
}

int rail_use_tile(rail_t rail, tile_t tile) {
  int i = 0;
  for (; i < TILES_ON_RAIL; ++i) {
    if (rail[i] == CHAR_TO_TILE(tile)) {
      rail[i] = EMPTY;
      return i;
    }
  }
  return TILE_NOT_FOUND;
}

int rail_use_tile_by_index(rail_t rail, int index) {
  if (index < 0 || index > TILES_ON_RAIL || rail[index] == EMPTY) { return TILE_NOT_FOUND; }
  else { rail[index] = EMPTY; return SUCCESS; }
}

int rail_empty(rail_t rail) {
  int i = 0;
  for (; i < TILES_ON_RAIL; ++i) {
    if (rail[i] != EMPTY) { return FALSE; }
  }
  return TRUE;
}
