#ifndef MOVE_H_
#define MOVE_H_

#define MAX_PLACEMENTS 7

struct placement_t {
  int row;
  int col;
  tile_t tile;
};

struct move_t {
  struct placement_t placements[MAX_PLACEMENTS];
  int n;
};

#endif
