#include "engine.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cinttypes> // TEMP TEMP
#include <algorithm> // TEMP TEMP
#include <memory> // TEMP TEMP

#define INFO(fmt, ...) fprintf(stderr, "ENGINE DEBUG: " fmt "\n", ##__VA_ARGS__);
#define ASIZE(x) (sizeof(x) / sizeof(x[0]))

typedef uint32_t u32;
typedef uint64_t u64;

// [1..26] => ['A'..'Z']
constexpr u32  ANYTILE = 0xffffffffu; // ;(1u << 26) - 1;
constexpr char EMPTY = 52;
constexpr int  BLANK = 26;
constexpr int  DIM = 15;
constexpr int  HORZ = 1;
constexpr int  VERT = DIM;
constexpr int  SQ_H8 = 112;
constexpr const char* const SquareNames[225] = {
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

constexpr const char* const DIR(int dir) noexcept {
    if (dir == HORZ) {
        return "HORZ";
    } else if (dir == VERT) {
        return "VERT";
    } else {
        return "UNKNOWN";
    }
}

constexpr const char* SQ(int sq) noexcept {
    if (0 <= sq && sq < 225) {
        return SquareNames[sq];
    }
    return "UNK";
}

const char* SQ_(int sq) noexcept { return SQ(sq); }

// return the internal tile #, ignores blank-ness; result always [0, 26)
constexpr char to_int(char tile) noexcept {
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

// return the engine representation for the tile
constexpr char to_eng(char tile) noexcept {
    if ('A' <= tile && tile <= 'Z') {
        return (tile - 'A');
    } else if ('a' <= tile && tile <= 'z') {
        return (tile - 'a') + BLANK;
    } else {
        assert(0 && "invalid tile");
        __builtin_unreachable();
        return 0;
    }
    // return to_int(tile);
}

constexpr char to_ext(char tile) noexcept {
    if (0 <= tile && tile < BLANK) {
        return tile + 'A';
    } else if (BLANK <= tile && tile < 2*BLANK){
        return (tile - BLANK) + 'a';
    } else {
        assert(0 && "invalid teng tile");
        __builtin_unreachable();
        return 0;
    }
}

constexpr int flip_dir(int d) noexcept {
    switch (d) {
        case HORZ: return VERT;
        case VERT: return HORZ;
    }
    assert(0 && "invalid direction");
    return 0;
}

constexpr u32 mask(char tile) {
    assert(0 <= tile && tile < 26);
    return 1u << tile;
}

// TEMP TEMP -- remove me
std::unique_ptr<char[]> mask_buffer(u32 m)
{
    auto buf = std::make_unique<char[]>(27);
    std::size_t bidx = 0;
    for (int i = 0; i < 26; ++i) {
        if ((m & mask(i)) != 0) {
            buf[bidx++] = to_ext(i);
        }
    }
    buf[bidx] = 0;
    return buf;
}

constexpr int getcol(int sq) noexcept { return sq % DIM; }
constexpr int getrow(int sq) noexcept { return sq / DIM; }
constexpr int getdim(int dir, int sq) noexcept { return dir == HORZ ? getrow(sq) : getcol(sq); } // TEMP TEMP
constexpr int rowstart(int sq) noexcept { return getcol(sq); }
constexpr int colstart(int sq) noexcept { return getrow(sq) * DIM; }

constexpr void setasq(u64* asq, int sq) noexcept
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    asq[m] |= static_cast<u64>(1ull << n);
}

constexpr void clrasq(u64* asq, int sq) noexcept
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    asq[m] &= ~static_cast<u64>(1ull << n);
}

constexpr int getasq(const u64* asq, int sq) noexcept
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    return (asq[m] & static_cast<u64>(1ull << n)) != 0;
}

void engine_init(Engine* e)
{
    memset(e->vals, EMPTY, sizeof(e->vals));
    memset(e->vchk, 0xffu, sizeof(e->vchk));
    memset(e->hchk, 0xffu, sizeof(e->hchk));
    memset(e->asqs, 0x00u, sizeof(e->asqs));
    const int H8 = 7*DIM + 7;
    setasq(e->asqs, H8);
}

constexpr int lsb(u64 x) noexcept { return __builtin_ctzll(x); }
constexpr u64 clearlsb(u64 x) noexcept { return x & (x - 1); }

struct Word
{
    char* buf;
    int   len;
};

const char* print_rack(const EngineRack* r)
{
    static char buf[8];
    memset(buf, 0, sizeof(buf));
    std::size_t bidx = 0;
    for (int tile = 0; tile < 26; ++tile) {
        for (int i = 0; i < r->tiles[tile]; ++i) {
            assert(bidx < 8);
            buf[bidx++] = to_ext(tile);
        }
    }
    for (int i = 0; i < r->tiles[BLANK]; ++i) {
        buf[bidx++] = '?';
    }
    buf[bidx] = 0;
    return buf;
}

void revbuf(char* buf, int length)
{
    char* p1 = &buf[0];
    char* p2 = &buf[length - 1];
    while (p1 < p2) {
        char t = *p1;
        *p1 = *p2;
        *p2 = t;
        ++p1;
        --p2;
    }
}

// Definitions:
//   + "Left Part"  -- prefix that goes BEFORE the anchor square, it may be empty
//   + "Right Part" -- rest of word, which must start at the anchor square
//
// Therefore:
//   + Potential left part is the minimum of the distance (going left) to the edge
//     of the board or the next left anchor extending to the current anchor
//   + All squares within the potential left part have trivial cross-checks
//   + Anchor square must be filled in to be a legal word
//
// Additional Notes:
//   + Everything in this file is specified from the standpoint of generating horizontal
//     moves. For vertical moves, we can just conceptually rotate the board.

int range_contains_square(int start, int stride, int length, int sq)
{
    for (int i = 0; i < length; ++i) {
        if (start + stride * i == sq) {
            return 1;
        }
    }
    return 0;
}

void extend_right(const Engine* e, int dir, int lsq, int sq, Word* word, EngineRack* r, int right_part_length, Word leftp, int anchor_orig)
{
    word->buf[word->len] = 0;
    const Edges edges_ = e->prefix_edges(e->prefix_edges_data, word->buf);
    const char* edges = edges_.edges;
    const bool terminal = edges_.terminal;
    const auto* vals = e->vals;
    const auto* xchk = dir == HORZ ? e->hchk : e->vchk;
    // TODO: maybe just pass in start/stride/stop into all of these?
    const int start = dir == HORZ ? colstart(lsq) : rowstart(lsq);
    const int stride = dir;
    const int stop  = start + stride * DIM;
    const int nextsq = sq + stride;
    const int rsq = sq - stride;
    auto* rack = r->tiles;

#if MYDEBUG
    // DEBUG
    if (anchor_orig == SQIX('O', 12)) {
        INFO("extend_right: dir=%s lsq=%s sq=%s word=\"%s\" rack=%s right_part_length=%d leftp=%s anchor_orig=%s edges=%s terminal=%s",
                DIR(dir), SQ(lsq), SQ(sq), word->buf, print_rack(r), right_part_length, leftp.buf, SQ(anchor_orig), edges, terminal?"TRUE":"FALSE");

        if (strcmp(word->buf, "SIR") == 0) {
            printf("HERE!\n");
        }
    }
    // END DEBUG
#endif

    if (sq >= stop || vals[sq] == EMPTY) {
        if (right_part_length > 0 && terminal) {
            assert(word->buf[word->len] == 0);
            e->on_legal_move(e->on_legal_move_data, word->buf, lsq, rsq, dir);
        }
        if (sq >= stop) { // hit end of board
            return;
        }
        for (const char* tile = edges; *tile != 0; ++tile) {
            const char tint = to_int(*tile);
            if (rack[tint] == 0) {               // have tile?
                continue;
            }
            if ((xchk[sq] & mask(tint)) == 0) {  // meets cross-check?
                continue;
            }
            rack[tint]--;
            word->buf[word->len++] = *tile; // TODO: hoist out of loop
            word->buf[word->len]   = 0;     // TODO: hoist out of loop
            assert(word->len <= DIM);
            extend_right(e, dir, lsq, nextsq, word, r, right_part_length + 1, leftp, anchor_orig);
            word->len--;                    // TODO: hoist out of loop
            word->buf[word->len] = 0;       // TODO: hoist out of loop
            rack[tint]++;
        }
        // NOTE: need to run a second time with blanks so I check both path of using
        //       the blank vs using the actual tile if I have it
        if (rack[BLANK] > 0) {
            for (const char* tile = edges; *tile != 0; ++tile) {
                const char tint = to_int(*tile);
                if ((xchk[sq] & mask(tint)) == 0) {  // meets cross-check?
                    continue;
                }
                rack[BLANK]--;                  // TODO: hoist out of loop
                word->buf[word->len++] = 'a' + (*tile - 'A'); // TODO: hoist len incr out of loop
                word->buf[word->len]   = 0;     // TODO: hoist out of loop
                assert(word->len <= DIM);
                extend_right(e, dir, lsq, nextsq, word, r, right_part_length + 1, leftp, anchor_orig);
                word->len--;                    // TODO: hoist out of loop
                word->buf[word->len] = 0;       // TODO: hoist out of loop
                rack[BLANK]++;                  // TODO: hoist out of loop
            }
        }
    } else {
        assert(vals[sq] != EMPTY);
        const char tile = vals[sq] < BLANK ? vals[sq] + 'A' : vals[sq] - BLANK + 'A'; // force to be in range [A..Z]
        assert('A' <= tile && tile <= 'Z');
        if (strchr(edges, tile) != NULL) {
            word->buf[word->len++] = to_ext(vals[sq]);
            word->buf[word->len]   = 0;
            extend_right(e, dir, lsq, nextsq, word, r, right_part_length, leftp, anchor_orig);
            word->len--;
        }
    }
}

// TEMP TEMP
char lpart_buf[32];

Word save_lpart(const Word* w) {
    memset(lpart_buf, 0, sizeof(lpart_buf));
    strcpy(lpart_buf, w->buf);
    // memcpy(lpart_buf, w->buf, w->len);
    Word result;
    result.buf = lpart_buf;
    result.len = w->len;
    assert(strcmp(result.buf, w->buf) == 0);
    return result;
}

// TODO: pass in a "tiles_used" parameter so can get rid of right_part_length?
// TODO: remove `sq` parameter, can calculate it from sq = anchor - strlen(word->buf) - 1 (see line 278 assertion below)
void left_part(const Engine* e, int dir, int anchor, int sq, int limit, Word* word, EngineRack* r, int anchor_orig)
{
    word->buf[word->len] = 0; // TEMP?
    const Edges edges_ = e->prefix_edges(e->prefix_edges_data, word->buf);
    const char* edges = edges_.edges;
    const bool terminal = edges_.terminal;
    const auto* xchk = dir == HORZ ? e->hchk : e->vchk;
    const int start = dir == HORZ ? colstart(sq) : rowstart(sq);
    const int stride = dir;
    const int stop  = start + stride * DIM;
    auto* rack = r->tiles;
    // assert(e->vals[sq] == EMPTY);
    assert((((anchor - sq) / stride) - 1) == strlen(word->buf));

#ifdef MYDEBUG
    // DEBUG
    if (anchor_orig == SQIX('O', 12)) {
        INFO("left_part: dir=%s anchor=%s sq=%s limit=%d word=\"%s\" rack=%s",
                DIR(dir), SQ(anchor), SQ(sq), limit, word->buf, print_rack(r));
    }
    // END DEBUG
#endif

    extend_right(e, dir, sq + stride, anchor, word, r, /*right_part_length*/0, save_lpart(word), anchor_orig);

    if (limit == 0) {
        return;
    }

    for (const char* tile = edges; *tile != 0; ++tile) {
        const char tint = to_int(*tile);
        if (rack[tint] == 0) {              // have tile?
            continue;
        }
        assert(xchk[sq] == ANYTILE); // see section 3.3.1 Placing Left Parts
        rack[tint]--;
        word->buf[word->len++] = *tile; // TODO: hoist incr of len
        word->buf[word->len]   = 0;     // TODO: hoist this out of loop
        assert(sq >= start);
        left_part(e, dir, anchor, sq - stride, limit - 1, word, r, anchor_orig); // try to expand the left part more to the left
        word->len--;                    // TODO: hoist out of loop
        word->buf[word->len] = 0;       // TODO: hoist this out of loop
        rack[tint]++;
    }
    if (rack[BLANK] > 0) {
        for (const char* tile = edges; *tile != 0; ++tile) {
            assert(xchk[sq] == ANYTILE); // see section 3.3.1 Placing Left Parts
            rack[BLANK]--;                        // TODO: hoist out of loop
            word->buf[word->len++] = 'a' + (*tile - 'A'); // TODO: hoist len incr out of loop
            word->buf[word->len]   = 0;           // TODO: hoist this out of loop
            assert(sq >= start);
            left_part(e, dir, anchor, sq - stride, limit - 1, word, r, anchor_orig); // try to expand the left part more to the left
            word->len--;                    // TODO: hoist out of loop
            word->buf[word->len] = 0;       // TODO: hoist this out of loop
            rack[BLANK]++;                  // TODO: hoist out of loop
        }
    }
}

void extend_right_on_existing_left_part(const Engine* e, int dir, EngineRack* r, int anchor, Word* word, int anchor_orig)
{
    const auto* vals = e->vals;
    const int start = dir == HORZ ? colstart(anchor) : rowstart(anchor);
    const int stride = dir;
    int sq = anchor - stride;
    assert(sq >= start);
    assert(vals[sq] != EMPTY);

#ifdef MYDEBUG
    // DEBUG
    if (anchor_orig == SQIX('O', 12)) {
        INFO("extend_right_on_existing_left_part: dir=%s anchor=%s word=\"%s\" rack=%s",
                DIR(dir), SQ(anchor), word->buf, print_rack(r));
    }
    // END DEBUG
#endif

    // TODO: more efficient way to extend backwards? maybe make the buffer (DIM+1)*2 so we can start
    //       in the middle?
    while (sq >= start && vals[sq] != EMPTY) {
        word->buf[word->len++] = to_ext(vals[sq]);
        sq -= stride;
    }
    word->buf[word->len] = 0;
    revbuf(word->buf, word->len);
    extend_right(e, dir, anchor - stride * word->len, anchor, word, r, 0, save_lpart(word), anchor_orig);
    word->len = 0;
}

void engine_find(const Engine* e, EngineRack rack)
{
    constexpr int dirs[] = { HORZ, VERT };

    printf("--- BEGIN ENGINE FIND --- rack = %s\n", print_rack(&rack));
    auto* asqs = e->asqs;
    auto* vals = e->vals;
    char buf[16];
    Word word;
    word.buf = buf;
    word.len = 0;
    word.buf[0] = 0;
    for (int i = 0; i < 4; ++i) {
        const int base = 64*i;
        u64 msk = asqs[i];
        while (msk > 0) {
            const int anchor = base + lsb(msk);
            for (int i = 0; i < ASIZE(dirs); ++i) {
                const int stride  = dirs[i];
                const int start = dirs[i] == HORZ ? colstart(anchor) : rowstart(anchor);
                if (anchor - stride >= start && vals[anchor - stride] != EMPTY) {
                    extend_right_on_existing_left_part(e, stride, &rack, anchor, &word, anchor);
                } else {
                    int limit = 0; // max left part potential length
                    for (int sq = anchor - stride; sq >= start; sq -= stride) {
                        if (getasq(asqs, sq) != 0) {
                            break;
                        }
                        if (vals[sq] != EMPTY) {
                            break;
                        }
                        ++limit;
                    }
                    const int left_most_poss_sq = anchor - stride * limit;
                    assert(left_most_poss_sq >= start);
                    assert((left_most_poss_sq == start) ||
                           (
                                (getasq(asqs, left_most_poss_sq) != 0) ||
                                (vals[left_most_poss_sq] == EMPTY)
                           ));
                    left_part(e, stride, anchor, anchor - stride, limit, &word, &rack, anchor);
                }
            }

            msk = clearlsb(msk);
        }
    }
    printf("--- END ENGINE FIND ---\n");
}

int engine_xchk(const Engine* e, const EngineMove* m)
{
    const int dir = m->direction;
    const int ntiles = m->ntiles;
    const int stride = flip_dir(dir);
    auto* tiles = m->tiles;
    auto* squares = m->squares;
    auto* vals = e->vals;
    auto* hchk = e->hchk;
    auto* vchk = e->vchk;
    auto* xchk = dir == HORZ ? hchk : vchk;
    for (int i = 0; i < ntiles; ++i) {
        const int sq   = squares[i];
        const int tile = tiles[i];
        const int tint = to_int(tile);
        const u32 msk = mask(tint);
        // INFO("sq=%s chk[%d]=0x%04x=%s=%u hchk[%d]=0x%04x=%s=%u vchk[%d]=0x%04x=%s=%u",
        //     SQ(sq),
        //     sq,  chk[sq], MBUF( chk[sq]), MM( chk[sq], msk),
        //     sq, hchk[sq], MBUF(hchk[sq]), MM(hchk[sq], msk),
        //     sq, vchk[sq], MBUF(vchk[sq]), MM(vchk[sq], msk)
        // );
        if ((xchk[sq] & mask(tint)) == 0) {
            return sq;
        }
    }
    return 0;
}

int findbeg(const char* vals, const int start, const int stop, const int stride, const int root)
{
    assert(vals[root] != EMPTY);
    int sq = root - stride;
    while (sq >= start && vals[sq] != EMPTY) {
        sq -= stride;
    }
    assert(vals[sq + stride] != EMPTY);
    return sq + stride;
}

int findend(const char* vals, const int start, const int stop, const int stride, const int root)
{
    assert(vals[root] != EMPTY);
    int sq = root + stride;
    while (sq < stop && vals[sq] != EMPTY) {
        sq += stride;
    }
    assert(vals[sq - stride] != EMPTY);
    return sq - stride;
}

int inclusive_length(int beg, int end, int stride) {
    assert(beg <= end);
    return (end - beg) / stride + 1;
}

void engine_make_move(Engine* e, const EngineMove* m)
{
    // NOTE(peter): everything in this function is named as if computing
    // the horizontal cross-checks, but it is actually direction agnotistic.
    char buf[17];
    const auto* tiles = m->tiles;
    const auto* squares = m->squares;
    const int dir = m->direction;
    const int ntiles = m->ntiles;
    const int hstride = m->direction;
    const int vstride = flip_dir(m->direction);
    auto* vals = e->vals;
    auto* hchk = dir == HORZ ? e->hchk : e->vchk;
    auto* vchk = dir == HORZ ? e->vchk : e->hchk;
    auto* asqs = e->asqs;
    auto* horzstart = dir == HORZ ? &colstart : &rowstart;
    auto* vertstart = dir == HORZ ? &rowstart : &colstart;
    const int lsq   = squares[0];          // left-most square
    const int rsq   = squares[ntiles - 1]; // right-most square
    const int hstart = horzstart(lsq);
    const int hstop  = hstart + hstride * DIM;
    assert(ntiles > 0);
    assert(squares != NULL);
    assert(tiles != NULL);
    assert(std::is_sorted(squares, squares+ntiles));

    // update vertical cross-checks
    for (int tidx = 0; tidx < ntiles; ++tidx) {
        const int stride = vstride;
        const int root = squares[tidx];
        const char tile = tiles[tidx];
        const char teng = to_eng(tile);
        const int start = vertstart(root);
        const int stop = start + DIM * stride;
        assert(vals[root] == EMPTY);
        vals[root] = teng;
        const int beg = findbeg(vals, start, stop, stride, root);
        const int end = findend(vals, start, stop, stride, root);
        const int len = inclusive_length(beg, end, stride);
        for (int i = 0; i < len; ++i) {
            buf[i+1] = to_ext(vals[beg + i*stride]);
            assert(('A' <= buf[i+1] && buf[i+1] <= 'Z') || ('a' <= buf[i+1] && buf[i+1] <= 'z'));
        }
        buf[len+1] = '\0';
        buf[len+2] = '\0';
        const int before = beg - stride;
        const int after  = end + stride;
        if (before >= start) {
            assert(getdim(stride, before) == getdim(stride, root));
            // TODO(peter): switch to using prefix call to get children
            u32 chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[0] = c;
                auto edges = e->prefix_edges(e->prefix_edges_data, buf);
                if (edges.terminal) {
                    chk |= mask(to_int(c));
                }
            }
            hchk[before] = chk;
            setasq(asqs, before);
        }

        if (after < stop) {
            assert(getdim(stride, after) == getdim(stride, root));
            assert(buf[len+1] == '\0');
            // TODO(peter): switch to using prefix call to get children
            u32 chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[len+1] = c;
                auto edges = e->prefix_edges(e->prefix_edges_data, &buf[1]);
                if (edges.terminal) {
                    chk |= mask(to_int(c));
                }
            }
            hchk[after] = chk;
            setasq(asqs, after);
        }

        clrasq(asqs, root);
    }

    { // update horizontal cross-checks
        const int start = hstart;
        const int stride  = hstride;
        const int stop  = hstop;
        const int beg = findbeg(vals, start, stop, stride, lsq);
        const int end = findend(vals, start, stop, stride, rsq);
        const int len = inclusive_length(beg, end, stride);
        assert(getdim(stride, lsq) == getdim(stride, rsq)); // move must be on exactly 1 row or col
        for (int i = 0; i < len; ++i) {
            buf[i+1] = to_ext(vals[beg+i*stride]);
            assert(('A' <= buf[i+1] && buf[i+1] <= 'Z') || ('a' <= buf[i+1] && buf[i+1] <= 'z'));
        }
        buf[len+1] = '\0';
        buf[len+2] = '\0';
        const int before = beg - stride;
        const int after  = end + stride;
        if (before >= start) {
            assert(vals[before] == EMPTY);
            assert(getdim(stride, before) == getdim(stride, lsq));
            assert(getdim(stride, before) == getdim(stride, rsq));
            // TODO(peter): switch to using prefix call to get children
            u32 chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[0] = c;
                auto edges = e->prefix_edges(e->prefix_edges_data, buf);
                if (edges.terminal) {
                    chk |= mask(to_int(c));
                }
            }
            vchk[before] = chk;
            setasq(asqs, before);
        }
        if (after < stop) {
            assert(vals[after] == EMPTY);
            assert(getdim(stride, after) == getdim(stride, lsq));
            assert(getdim(stride, after) == getdim(stride, rsq));
            assert(buf[len+1] == '\0');
            // TODO(peter): switch to using prefix call to get children
            u32 chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[len+1] = c;
                auto edges = e->prefix_edges(e->prefix_edges_data, &buf[1]);
                if (edges.terminal) {
                    chk |= mask(to_int(c));
                }
            }
            vchk[after] = chk;
            setasq(asqs, after);
        }
    }
}

void engine_print_anchors(const Engine* e)
{
    auto* asqs = e->asqs;
    auto* vals = e->vals;
    auto getcc = [asqs, vals](int row, int col) {
        int idx = row*DIM + col;
        return getasq(asqs, idx) != 0 ? '*' : (vals[idx] == EMPTY ? ' ' : to_ext(vals[idx]));
    };

    printf("     1   2   3   4   5   6   7   8   9   0   1   2   3   4   5  \n");
    printf("   -------------------------------------------------------------\n");
    for (int row = 0; row < DIM; ++row) {
        printf("%c  | ", static_cast<char>('A' + row));
        for (int col = 0; col < DIM - 1; ++col) {
            printf("%c | ", getcc(row, col));
        }
        printf("%c", getcc(row, DIM-1));
        printf(" |\n");
        printf("   -------------------------------------------------------------\n");
    }
    printf("\n");
}
