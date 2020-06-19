#ifndef CICERO_TYPES__H_
#define CICERO_TYPES__H_

#include <cicero/cicero.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


// TEMP TEMP
#define DO_TRACE
#ifdef DO_TRACE
#include <stdio.h>
#define TRACE(fmt, ...) fprintf(stderr, "[TRACE (%s)]: " fmt "\n", __func__, ##__VA_ARGS__);
#else
#define TRACE(fmt, ...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define internal static inline

#define ASIZE(x) (sizeof(x) / sizeof(x[0]))


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

static const u32  ANYTILE = 0xffffffffu;
static const char EMPTY = 52;
static const int  BLANK = 26;
static const int  DIM = 15;
static const int  HORZ = CICERO_HORZ;
static const int  VERT = CICERO_VERT;
static const int  SQ_H8 = 112;
static const int  SQ_I8 = 127;
// TODO: better way to do this?
// need a sentinel to indicate that there are no touching cross tiles
// because a blank is worth 0. In the situation of forming a cross with
// blank tile, still need to double count the placed tile's value.
static const u16  NOCROSSTILES = 0xffffu;

// Tile classes:
typedef int  rack_tile; // 0-26 (26 == Blank)
typedef char eng_tile; // 0-25 = regular tile, 26-51 = blank tile
typedef char ext_tile; // A-Z = regular tile, a-z = blank tile
typedef char tile_num; // 0-25 ignoring blankness, i.e. A=a=0

typedef int (*dimstart)(int sq);

internal int getcol(int sq)   { return sq % DIM; }
internal int getrow(int sq)   { return sq / DIM; }
internal int getdim(int dir, int sq) { return dir == HORZ ? getrow(sq) : getcol(sq); } // TEMP TEMP
// TODO: rename these; they are backwards
internal int rowstart(int sq) { return getcol(sq); }
internal int colstart(int sq) { return getrow(sq) * DIM; }
internal int lsb(u64 x)       { return __builtin_ctzll(x); }
internal u64 clearlsb(u64 x)  { return x & (x - 1); }

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

internal int flip_dir(int d)
{
    switch (d) {
        case CICERO_HORZ: return VERT;
        case CICERO_VERT: return HORZ;
    }
    return 0;
}

internal u32 tilemask(char tile)
{
    assert(0 <= tile && tile < 26);
    return 1u << tile;
}

// TODO: make table based
internal eng_tile to_eng(char tile)
{
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
internal ext_tile to_ext(eng_tile tile)
{
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

internal ext_tile to_uppercase(ext_tile tile)
{
    if ('A' <= tile && tile <= 'Z') {
        return tile;
    } else if ('a' <= tile && tile <= 'z') {
        return 'A' + (tile - 'a');
    } else {
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

// precondition: `root` is the left-most square trying to be played
internal int findbeg(const char* vals, const int start, const int stop,
        const int stride, const int root)
{
    int sq = root - stride;
    while (sq >= start && vals[sq] != EMPTY) {
        sq -= stride;
    }
    sq += stride;
    assert(vals[sq] != EMPTY || sq == root);
    return sq;
}

// precondition: `root` should be the right-most square trying to be played
internal int findend(const char* vals, const int start, const int stop,
        const int stride, const int root)
{
    int sq = root + stride;
    while (sq < stop && vals[sq] != EMPTY) {
        sq += stride;
    }
    sq -= stride;
    assert(vals[sq] != EMPTY || sq == root);
    return sq;
}


struct scoreresult_
{
    int score;
    int tiles;
};
typedef struct scoreresult_ scoreresult;

internal scoreresult scoreleft(const cicero* e, const int start,
        const int stop, const int stride, const int root)
{
    const char *vals = e->vals;
    const int  *letter_values = e->s.letter_values;
    int score = 0;
    int tiles = 0;
    int sq = root - stride;
    while (sq >= start && vals[sq] != EMPTY) {
        assert(0 <= sq && sq < 225);
        score += letter_values[vals[sq]];
        sq    -= stride;
        ++tiles;
    }
    scoreresult result = { .score=score, .tiles=tiles };
    return result;
}

internal scoreresult scoreright(const cicero* e, const int start,
        const int stop, const int stride, const int root)
{
    const char *vals = e->vals;
    const int  *letter_values = e->s.letter_values;
    int score = 0;
    int tiles = 0;
    int sq = root + stride;
    while (sq < stop && vals[sq] != EMPTY) {
        assert(0 <= sq && sq < 225);
        score += letter_values[vals[sq]];
        sq    += stride;
        ++tiles;
    }
    scoreresult result = { .score=score, .tiles=tiles };
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CICERO_TYPES__H_
