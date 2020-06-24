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

#define FOR_TEST_COMPLIANCE

#ifdef __cplusplus
extern "C" {
#endif

#define internal static inline

#define ASIZE(x) (sizeof(x) / sizeof(x[0]))


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

enum {
    SQ_A1, SQ_A2, SQ_A3, SQ_A4, SQ_A5, SQ_A6, SQ_A7, SQ_A8, SQ_A9, SQ_A10, SQ_A11, SQ_A12, SQ_A13, SQ_A14, SQ_A15,
    SQ_B1, SQ_B2, SQ_B3, SQ_B4, SQ_B5, SQ_B6, SQ_B7, SQ_B8, SQ_B9, SQ_B10, SQ_B11, SQ_B12, SQ_B13, SQ_B14, SQ_B15,
    SQ_C1, SQ_C2, SQ_C3, SQ_C4, SQ_C5, SQ_C6, SQ_C7, SQ_C8, SQ_C9, SQ_C10, SQ_C11, SQ_C12, SQ_C13, SQ_C14, SQ_C15,
    SQ_D1, SQ_D2, SQ_D3, SQ_D4, SQ_D5, SQ_D6, SQ_D7, SQ_D8, SQ_D9, SQ_D10, SQ_D11, SQ_D12, SQ_D13, SQ_D14, SQ_D15,
    SQ_E1, SQ_E2, SQ_E3, SQ_E4, SQ_E5, SQ_E6, SQ_E7, SQ_E8, SQ_E9, SQ_E10, SQ_E11, SQ_E12, SQ_E13, SQ_E14, SQ_E15,
    SQ_F1, SQ_F2, SQ_F3, SQ_F4, SQ_F5, SQ_F6, SQ_F7, SQ_F8, SQ_F9, SQ_F10, SQ_F11, SQ_F12, SQ_F13, SQ_F14, SQ_F15,
    SQ_G1, SQ_G2, SQ_G3, SQ_G4, SQ_G5, SQ_G6, SQ_G7, SQ_G8, SQ_G9, SQ_G10, SQ_G11, SQ_G12, SQ_G13, SQ_G14, SQ_G15,
    SQ_H1, SQ_H2, SQ_H3, SQ_H4, SQ_H5, SQ_H6, SQ_H7, SQ_H8, SQ_H9, SQ_H10, SQ_H11, SQ_H12, SQ_H13, SQ_H14, SQ_H15,
    SQ_I1, SQ_I2, SQ_I3, SQ_I4, SQ_I5, SQ_I6, SQ_I7, SQ_I8, SQ_I9, SQ_I10, SQ_I11, SQ_I12, SQ_I13, SQ_I14, SQ_I15,
    SQ_J1, SQ_J2, SQ_J3, SQ_J4, SQ_J5, SQ_J6, SQ_J7, SQ_J8, SQ_J9, SQ_J10, SQ_J11, SQ_J12, SQ_J13, SQ_J14, SQ_J15,
    SQ_K1, SQ_K2, SQ_K3, SQ_K4, SQ_K5, SQ_K6, SQ_K7, SQ_K8, SQ_K9, SQ_K10, SQ_K11, SQ_K12, SQ_K13, SQ_K14, SQ_K15,
    SQ_L1, SQ_L2, SQ_L3, SQ_L4, SQ_L5, SQ_L6, SQ_L7, SQ_L8, SQ_L9, SQ_L10, SQ_L11, SQ_L12, SQ_L13, SQ_L14, SQ_L15,
    SQ_M1, SQ_M2, SQ_M3, SQ_M4, SQ_M5, SQ_M6, SQ_M7, SQ_M8, SQ_M9, SQ_M10, SQ_M11, SQ_M12, SQ_M13, SQ_M14, SQ_M15,
    SQ_N1, SQ_N2, SQ_N3, SQ_N4, SQ_N5, SQ_N6, SQ_N7, SQ_N8, SQ_N9, SQ_N10, SQ_N11, SQ_N12, SQ_N13, SQ_N14, SQ_N15,
    SQ_O1, SQ_O2, SQ_O3, SQ_O4, SQ_O5, SQ_O6, SQ_O7, SQ_O8, SQ_O9, SQ_O10, SQ_O11, SQ_O12, SQ_O13, SQ_O14, SQ_O15,
    NUM_SQUARES
};
_Static_assert(SQ_A1  ==   0);
_Static_assert(SQ_H8  == 112);
_Static_assert(SQ_I8  == 127);
_Static_assert(SQ_O15 == 224);

static const char* const SQ[225] = {
    " A1", " A2", " A3", " A4", " A5", " A6", " A7", " A8", " A9", "A10", "A11", "A12", "A13", "A14", "A15",
    " B1", " B2", " B3", " B4", " B5", " B6", " B7", " B8", " B9", "B10", "B11", "B12", "B13", "B14", "B15",
    " C1", " C2", " C3", " C4", " C5", " C6", " C7", " C8", " C9", "C10", "C11", "C12", "C13", "C14", "C15",
    " D1", " D2", " D3", " D4", " D5", " D6", " D7", " D8", " D9", "D10", "D11", "D12", "D13", "D14", "D15",
    " E1", " E2", " E3", " E4", " E5", " E6", " E7", " E8", " E9", "E10", "E11", "E12", "E13", "E14", "E15",
    " F1", " F2", " F3", " F4", " F5", " F6", " F7", " F8", " F9", "F10", "F11", "F12", "F13", "F14", "F15",
    " G1", " G2", " G3", " G4", " G5", " G6", " G7", " G8", " G9", "G10", "G11", "G12", "G13", "G14", "G15",
    " H1", " H2", " H3", " H4", " H5", " H6", " H7", " H8", " H9", "H10", "H11", "H12", "H13", "H14", "H15",
    " I1", " I2", " I3", " I4", " I5", " I6", " I7", " I8", " I9", "I10", "I11", "I12", "I13", "I14", "I15",
    " J1", " J2", " J3", " J4", " J5", " J6", " J7", " J8", " J9", "J10", "J11", "J12", "J13", "J14", "J15",
    " K1", " K2", " K3", " K4", " K5", " K6", " K7", " K8", " K9", "K10", "K11", "K12", "K13", "K14", "K15",
    " L1", " L2", " L3", " L4", " L5", " L6", " L7", " L8", " L9", "L10", "L11", "L12", "L13", "L14", "L15",
    " M1", " M2", " M3", " M4", " M5", " M6", " M7", " M8", " M9", "M10", "M11", "M12", "M13", "M14", "M15",
    " N1", " N2", " N3", " N4", " N5", " N6", " N7", " N8", " N9", "N10", "N11", "N12", "N13", "N14", "N15",
    " O1", " O2", " O3", " O4", " O5", " O6", " O7", " O8", " O9", "O10", "O11", "O12", "O13", "O14", "O15",
};

#define SQNAME(x) (((0 <= (x)) && ((x) < 225)) ? SQ[x] : "INV")

static const u32  ANYTILE = 0xffffffffu;
static const char EMPTY = 52;
static const int  BLANK = 26;
static const int  DIM = 15;
static const int  HORZ = CICERO_HORZ;
static const int  VERT = CICERO_VERT;
// static const int  SQ_H8 = 112;
// static const int  SQ_I8 = 127;
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
        assert(0 && "invalid teng tile");
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
