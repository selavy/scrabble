#ifndef CICERO_TYPES__H_
#define CICERO_TYPES__H_

#include <cicero/cicero.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define internal static inline

#define ASIZE(x) (sizeof(x) / sizeof(x[0]))

typedef uint32_t u32;
typedef uint64_t u64;

const u32  ANYTILE = 0xffffffffu;
const char EMPTY = 52;
const int  BLANK = 26;
const int  DIM = 15;
const int  HORZ = CICERO_HORZ;
const int  VERT = CICERO_VERT;
const int  SQ_H8 = 112;

// Tile classes:
typedef int  rack_tile; // 0-26 (26 == Blank)
typedef char eng_tile; // 0-25 = regular tile, 26-51 = blank tile
typedef char ext_tile; // A-Z = regular tile, a-z = blank tile
typedef char tile_num; // 0-25 ignoring blankness, i.e. A=a=0

internal int getcol(int sq)   { return sq % DIM; }
internal int getrow(int sq)   { return sq / DIM; }
internal int getdim(int dir, int sq) { return dir == HORZ ? getrow(sq) : getcol(sq); } // TEMP TEMP
internal int rowstart(int sq) { return getcol(sq); }
internal int colstart(int sq) { return getrow(sq) * DIM; }
internal int lsb(u64 x)       { return __builtin_ctzll(x); }
internal u64 clearlsb(u64 x)  { return x & (x - 1); }

typedef int (*dimstart)(int sq);

internal void setasq(u64* asq, int sq)
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    asq[m] |= (u64)(1ull << n);
}

internal void clrasq(u64* asq, int sq)
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    asq[m] &= ~((u64)(1ull << n));
}

internal int getasq(const u64* asq, int sq)
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    return (asq[m] & ((u64)(1ull << n))) != 0;
}

internal int flip_dir(int d) {
    switch (d) {
        case CICERO_HORZ: return VERT;
        case CICERO_VERT: return HORZ;
    }
    return 0;
}

internal u32 tilemask(char tile) {
    assert(0 <= tile && tile < 26);
    return 1u << tile;
}

// TODO: make table based
internal eng_tile to_eng(char tile) {
    if ('A' <= tile && tile <= 'Z') {
        return (tile - 'A');
    } else if ('a' <= tile && tile <= 'z') {
        return (tile - 'a') + BLANK;
    } else {
        assert(0 && "invalid tile");
        __builtin_unreachable();
        return 0;
    }
}

// TODO: make table based
internal ext_tile to_ext(eng_tile tile) {
    if (0 <= tile && tile < BLANK) {
        return tile + 'A';
    } else if (BLANK <= tile && tile < 2*BLANK) {
        return (tile - BLANK) + 'a';
    } else if (tile == EMPTY) {
        return CICERO_EMPTY_TILE;
    } else {
        // assert(0 && "invalid teng tile");
        __builtin_unreachable();
        return 0;
    }
}

internal tile_num tilenum(char tile) {
    if ('A' <= tile && tile <= 'Z') {
        return (tile - 'A');
    } else if ('a' <= tile && tile <= 'z') {
        return (tile - 'a');
    } else {
        assert(0 && "invalid tile");
        __builtin_unreachable();
        return 0;
    }
}

#endif // CICERO_TYPES__H_
