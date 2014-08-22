#ifndef _RAIL_TYPE_H_
#define _RAIL_TYPE_H_

#include "general.h"
#include <string.h> // for memset

#ifdef _DEBUG
#include <stdio.h>
#endif

#define TILES_ON_RAIL 7
#define TILE_NOT_FOUND -3

// TODO: could put the rail in a uint64_t
// 26 letters + 1 blank + 1 no letter = 28 states -> 5 bits
// (5 bits / letter) * (7 letters / rail) = 35 bits
//
// faster might be (8 bits / letter) * (7 letters / rail) = 56 bits
typedef tile_t rail_t[TILES_ON_RAIL];
//typedef tile_t * rail_t;

extern void rails_create(rail_t * rail, int n);
extern void rail_create(rail_t rail);
extern int rail_add_tile(rail_t rail, tile_t tile);
extern void rail_destroy(rail_t rail);
extern void rails_destroy(rail_t * rail, int n);
extern void rail_print(rail_t rail);
extern int rail_find_tile(rail_t rail, tile_t tile);
extern int rail_use_tile(rail_t rail, tile_t tile);
extern int rail_use_tile_by_index(rail_t rail, int index);
extern int rail_empty(rail_t rail);

#endif
