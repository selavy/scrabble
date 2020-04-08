#include "engine.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cinttypes> // TEMP TEMP
#include <algorithm> // TEMP TEMP
#include <memory> // TEMP TEMP

#define INFO(fmt, ...) fprintf(stderr, "ENGINE DEBUG: " fmt "\n", ##__VA_ARGS__);
#define ASIZE(x) (sizeof(x) / sizeof(x[0]))

// [1..26] => ['A'..'Z']
constexpr uint32_t ANYTILE = 0xffffffffu; // ;(1u << 26) - 1;
constexpr char     EMPTY = 26;
constexpr int      BLANK = 26;
constexpr int      DIM = 15;
constexpr int      HORZ = 1;
constexpr int      VERT = DIM;
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


/*constexpr*/ int SQIX(char row, int col) noexcept {
    int r = row - 'A';
    int c = col - 1;
    return r*DIM + c;
}

constexpr const char* SQ(int sq) noexcept {
    if (0 <= sq && sq < 225) {
        return SquareNames[sq];
    }
    return "UNK";
}

const char* SQ_(int sq) noexcept { return SQ(sq); }

constexpr char to_int(char tile) noexcept {
    if ('A' <= tile && tile <= 'Z') {
        return (tile - 'A');
    } else if ('a' <= tile && tile <= 'z') {
        return (tile - 'a');
    } else if (tile == ' ') {
        // TODO(peter): not sure I should ever hit this path here.
        return EMPTY;
    } else {
        assert(tile && "invalid tile");
        return 0;
    }
}

constexpr char to_ext(char tile) noexcept {
    if (tile == BLANK) {
        return tile + '?';
    } else {
        return tile + 'A';
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

constexpr uint32_t mask(char tile) {
    assert(0 <= tile && tile < 26);
    return 1u << tile;
}

// TEMP TEMP
std::unique_ptr<char[]> mask_buffer(uint32_t m) {
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

#define MBUF(m) mask_buffer(m).get()
#define MM(x, m) ((((x) & (m)) != 0) ? 1 : 0)
// TEMP TEMP

constexpr int getcol(int sq) noexcept { return sq % DIM; }
constexpr int getrow(int sq) noexcept { return sq / DIM; }
constexpr int getdim(int dir, int sq) noexcept { return dir == HORZ ? getrow(sq) : getcol(sq); } // TEMP TEMP
constexpr int rowstart(int sq) noexcept { return getcol(sq); }
constexpr int colstart(int sq) noexcept { return getrow(sq) * DIM; }

constexpr void setasq(uint64_t* asq, int sq) noexcept
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    asq[m] |= static_cast<uint64_t>(1ull << n);
}

constexpr void clrasq(uint64_t* asq, int sq) noexcept
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    asq[m] &= ~static_cast<uint64_t>(1ull << n);
}

constexpr int getasq(const uint64_t* asq, int sq) noexcept
{
    assert(0 <= sq < 225);
    const int m = sq / 64;
    const int n = sq % 64;
    return (asq[m] & static_cast<uint64_t>(1ull << n)) != 0;
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

constexpr int lsb(uint64_t x) noexcept { return __builtin_ctzll(x); }
constexpr uint64_t clearlsb(uint64_t x) noexcept { return x & (x - 1); }

struct Word
{
    char* buf;
    int   len;
};

#define ISWORD(word) (e->wordchk(e->wordchk_data, word) != 0)
#define ONLEGAL(word, sq, dir) e->on_legal_move(e->on_legal_move_data, word, sq, dir)

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

void extend_right(const Engine* e, const int anchor, int sq, Word* word, EngineRack* r, int right_part_length, const Word leftp)
{
    word->buf[word->len] = 0;
    const Edges edges_ = e->prefix_edges(e->prefix_edges_data, word->buf);
    const char* edges = edges_.edges;
    const bool  terminal = edges_.terminal;
    const auto* xchk = e->hchk;
    const int start = colstart(sq);
    const int step = HORZ;
    const int stop  = start + step*DIM;
    const int nextsq = sq + step;
    auto* rack = r->tiles;

    if (strcmp(leftp.buf, "ZAG") == 0) {
        // if (anchor == SQIX('I', 7)) {
        INFO("extend_right: word=\"%s\" left_part=\"%s\" sq=%s (%d) rack=%s edges=%s terminal=%s xchk=%s",
                word->buf, leftp.buf, SQ(sq), sq, print_rack(r), edges, (terminal?"TRUE":"FALSE"), MBUF(xchk[sq]));
    }

    if (e->vals[sq] == EMPTY) {
        if (right_part_length > 0 && terminal) {
            assert(word->buf[word->len] == 0);
            printf("!!! LEGAL MOVE: anchor=%s sq=%s dir=HORZ word=\"%s\"\n", SQ(anchor), SQ(sq), word->buf);
            // ONLEGAL(word->buf, anchor, HORZ);
        }
        if (nextsq >= stop) { // hit end of board
            return;
        }
        for (const char* tile = edges; *tile != 0; ++tile) {
            const char tint = to_int(*tile);
            if (rack[tint] == 0) {              // have tile?
                continue;
            }
            if ((xchk[sq] & mask(tint)) == 0) { // meets cross-check?
                continue;
            }
            rack[tint]--;
            word->buf[word->len++] = *tile;
            word->buf[word->len]   = 0;
            // INFO("\tPLAYING TILE: %c ON %s -- %s xchk=%s", *tile, SQ(sq), word->buf, MBUF(xchk[sq]));
            assert(word->len <= DIM);
            extend_right(e, anchor, nextsq, word, r, right_part_length + 1, leftp);
            word->len--;
            word->buf[word->len] = 0;
            rack[tint]++;
        }

        // TODO: handle blanks
        // if (rack[BLANK] > 0) {
        //     // ...
        // }
    } else if (*edges != 0 || terminal) {
        word->buf[word->len++] = to_ext(e->vals[sq]);
        word->buf[word->len] = 0; // TEMP TEMP
        // INFO("\tADDED ALREADY PLAYED TILE: %c -> %s", to_ext(e->vals[sq]), word->buf);
        if (nextsq < stop) {
            extend_right(e, anchor, nextsq, word, r, right_part_length, leftp);
        } else if ((right_part_length > 0) && terminal) {  // hit end of board with a valid word
            assert(word->buf[word->len] == 0);
            // ONLEGAL(word->buf, anchor, HORZ);
            printf("!!! LEGAL MOVE: anchor=%s sq=%s dir=HORZ word=\"%s\"\n", SQ(anchor), SQ(sq), word->buf);
        }
        word->len--;
    }
}

// definitions:
// "Left Part"  -- prefix that goes BEFORE the anchor square, it may be empty
// "Right Part" -- rest of word, which must start at the anchor square
//
// Therefore:
//   + potential left part is the minimum of the distance (going left) to the edge
//     of the board or the next left anchor extending to the current anchor
//   + all squares within the potential left part have trivial cross-checks
//   + anchor square must be filled in to be a legal word

void left_part(const Engine* e, const int anchor, int sq, int limit, Word* word, EngineRack* r)
{
    // sq is where next letter in potential left part will go

    word->buf[word->len] = 0; // TEMP?
    const Edges edges_ = e->prefix_edges(e->prefix_edges_data, word->buf);
    const char* edges = edges_.edges;
    const bool  terminal = edges_.terminal;
    const auto* xchk = e->hchk;
    const int start = colstart(sq);
    const int step = HORZ;
    const int stop  = start + step*DIM;
    auto* rack = r->tiles;

    // INFO("left_part: word=\"%s\" @ %s lmt=%d anchor=%s rack=%s edges=%s",
    //         word->buf, SQ(sq), limit, SQ(anchor), print_rack(r), edges);

    assert(e->vals[sq] == EMPTY);
    // assert((anchor - sq) == strlen(word->buf));
    extend_right(e, anchor, anchor, word, r, /*right_part_length*/0, *word);
    if (limit == 0) {
        return;
    }

    // INFO("left_part (after extend_right): word=%s (len=%d) anchor=%s sq=%s rack=%s edges=%s",
    //         word->buf, word->len, SQ(anchor), SQ(sq), print_rack(r), edges);

    for (const char* tile = edges; *tile != 0; ++tile) {
        const char tint = to_int(*tile);
        if (rack[tint] == 0) {              // have tile?
            continue;
        }
        assert(xchk[sq] == ANYTILE); // see section 3.3.1 Placing Left Parts
        rack[tint]--;
        word->buf[word->len++] = *tile;
        word->buf[word->len]   = 0;     // TODO: hoist this out of loop
        assert(sq >= start);
        left_part(e, anchor, sq - step, limit - 1, word, r);
        word->len--;
        word->buf[word->len] = 0;       // TODO: hoist this out of loop
        rack[tint]++;
    }
}

void extend_right_on_existing_left_part(const Engine* e, EngineRack* r, int anchor, Word* word)
{
    const auto* vals = e->vals;
    const int start = colstart(anchor);
    const int step = HORZ;
    int sq = anchor - step;
    assert(sq >= start);
    assert(vals[sq] != EMPTY);
    // TODO: more efficient way to extend backwards? maybe make the buffer (DIM+1)*2 so we can start
    //       in the middle?
    while (sq >= start && vals[sq] != EMPTY) {
        word->buf[word->len++] = to_ext(vals[sq]);
        sq -= step;
    }
    word->buf[word->len] = 0;
    revbuf(word->buf, word->len);
    INFO("extending existing left part: '%s'", word->buf);
    extend_right(e, anchor, /*sq*/anchor, word, r, 0, *word);
    word->len = 0;
}

void engine_find(const Engine* e, EngineRack rack)
{
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
        uint64_t msk = asqs[i];
        while (msk > 0) {
            int anchor = base + lsb(msk);
            // printf("HORIZONTAL ANCHOR: %s (%d)\n", SQ(anchor), anchor);
            const int start = colstart(anchor);
            const int step  = HORZ;
            int limit = 0; // max left part potential length
            for (int sq = anchor - step; sq >= start; sq -= step) {
                if (getasq(asqs, sq) != 0) {
                    break;
                }
                if (vals[sq] != EMPTY) {
                    break;
                }
                ++limit;
            }
            assert((anchor - step * limit) >= start);
            assert(((anchor - step * limit) == start) || (getasq(asqs, anchor - step * limit) != 0));
            if (anchor - step >= start && vals[anchor - step] != EMPTY) {
                extend_right_on_existing_left_part(e, &rack, anchor, &word);
            } else {
                left_part(e, anchor, anchor - step, limit, &word, &rack);
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
    const int step = flip_dir(dir);
    auto* tiles = m->tiles;
    auto* squares = m->squares;
    auto* vals = e->vals;
    auto* chk = dir == HORZ ? e->hchk : e->vchk;
    auto* hchk = e->hchk;
    auto* vchk = e->vchk;
    for (int i = 0; i < ntiles; ++i) {
        const int sq   = squares[i];
        const int tile = tiles[i];
        const int tint = to_int(tile);
        const uint32_t msk = mask(tint);
        // INFO("sq=%s chk[%d]=0x%04x=%s=%u hchk[%d]=0x%04x=%s=%u vchk[%d]=0x%04x=%s=%u",
        //     SQ(sq),
        //     sq,  chk[sq], MBUF( chk[sq]), MM( chk[sq], msk),
        //     sq, hchk[sq], MBUF(hchk[sq]), MM(hchk[sq], msk),
        //     sq, vchk[sq], MBUF(vchk[sq]), MM(vchk[sq], msk)
        // );
        if ((chk[sq] & mask(tint)) == 0) {
            return sq;
        }
    }
    return 0;
}

int findbeg(const char* vals, const int start, const int stop, const int step, const int root)
{
    assert(vals[root] != EMPTY);
    int sq = root - step;
    while (sq >= start && vals[sq] != EMPTY) {
        sq -= step;
    }
    assert(vals[sq + step] != EMPTY);
    return sq + step;
}

int findend(const char* vals, const int start, const int stop, const int step, const int root)
{
    assert(vals[root] != EMPTY);
    int sq = root + step;
    while (sq < stop && vals[sq] != EMPTY) {
        sq += step;
    }
    assert(vals[sq - step] != EMPTY);
    return sq - step;
}

int inclusive_length(int beg, int end, int step) {
    assert(beg <= end);
    return (end - beg) / step + 1;
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
    const int hstep = m->direction;
    const int vstep = flip_dir(m->direction);
    auto* vals = e->vals;
    auto* hchk = dir == HORZ ? e->hchk : e->vchk;
    auto* vchk = dir == HORZ ? e->vchk : e->hchk;
    auto* asqs = e->asqs;
    auto* horzstart = dir == HORZ ? &colstart : &rowstart;
    auto* vertstart = dir == HORZ ? &rowstart : &colstart;
    auto* chkwrd = e->wordchk;
    const int lsq   = squares[0];          // left-most square
    const int rsq   = squares[ntiles - 1]; // right-most square
    const int hstart = horzstart(lsq);
    const int hstop  = hstart + hstep * DIM;
    assert(ntiles > 0);
    assert(squares != NULL);
    assert(tiles != NULL);
    assert(std::is_sorted(squares, squares+ntiles));

    // update vertical cross-checks
    for (int tidx = 0; tidx < ntiles; ++tidx) {
        const int step = vstep;
        const int root = squares[tidx];
        const char tile = tiles[tidx];
        const char tint = to_int(tile);
        const int start = vertstart(root);
        const int stop = start + DIM * step;
        assert(vals[root] == EMPTY);
        vals[root] = tint;
        const int beg = findbeg(vals, start, stop, step, root);
        const int end = findend(vals, start, stop, step, root);
        const int len = inclusive_length(beg, end, step);
        for (int i = 0; i < len; ++i) {
            buf[i+1] = to_ext(vals[beg + i*step]);
            assert('A' <= buf[i+1] && buf[i+1] <= 'Z');
        }
        buf[len+1] = '\0';
        buf[len+2] = '\0';
        const int before = beg - step;
        const int after  = end + step;
        if (before >= start) {
            assert(getdim(step, before) == getdim(step, root));
            // TODO(peter): switch to using prefix call to get children
            uint32_t chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[0] = c;
                if (chkwrd(e->wordchk_data, buf) != 0) {
                    chk |= mask(to_int(c));
                }
            }
            hchk[before] = chk;
            setasq(asqs, before);
        }

        if (after < stop) {
            assert(getdim(step, after) == getdim(step, root));
            assert(buf[len+1] == '\0');
            // TODO(peter): switch to using prefix call to get children
            uint32_t chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[len+1] = c;
                if (chkwrd(e->wordchk_data, &buf[1]) != 0) {
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
        const int step  = hstep;
        const int stop  = hstop;
        const int beg = findbeg(vals, start, stop, step, lsq);
        const int end = findend(vals, start, stop, step, rsq);
        const int len = inclusive_length(beg, end, step);
        assert(getdim(step, lsq) == getdim(step, rsq)); // move must be on exactly 1 row or col
        for (int i = 0; i < len; ++i) {
            buf[i+1] = to_ext(vals[beg+i*step]);
            assert('A' <= buf[i+1] && buf[i+1] <= 'Z');
        }
        buf[len+1] = '\0';
        buf[len+2] = '\0';
        const int before = beg - step;
        const int after  = end + step;
        if (before >= start) {
            assert(vals[before] == EMPTY);
            assert(getdim(step, before) == getdim(step, lsq));
            assert(getdim(step, before) == getdim(step, rsq));
            // TODO(peter): switch to using prefix call to get children
            uint32_t chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[0] = c;
                if (chkwrd(e->wordchk_data, buf) != 0) {
                    chk |= mask(to_int(c));
                }
            }
            vchk[before] = chk;
            setasq(asqs, before);
        }
        if (after < stop) {
            assert(vals[after] == EMPTY);
            assert(getdim(step, after) == getdim(step, lsq));
            assert(getdim(step, after) == getdim(step, rsq));
            assert(buf[len+1] == '\0');
            // TODO(peter): switch to using prefix call to get children
            uint32_t chk = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                buf[len+1] = c;
                if (chkwrd(e->wordchk_data, &buf[1]) != 0) {
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
        return getasq(asqs, idx) != 0 ? '*' : to_ext(vals[idx]);
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
