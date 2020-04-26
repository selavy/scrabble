#include "cicero_types.h"
#include "cicero_tables.h"

internal rack_tile char_to_rack_tile(char tile) {
    assert(('A' <= tile && tile <= 'Z') || tile == CICERO_BLANK_TILE);
    return tile == CICERO_BLANK_TILE ? 26 : tile - 'A';
}

internal int inclusive_length(int beg, int end, int stride) {
    assert(beg <= end);
    return (end - beg) / stride + 1;
}

char cicero_tile_on_square(const cicero *e, int square)
{
    assert(0 <= square && square < DIM*DIM);
    return to_ext(e->vals[square]);
}

void cicero_init(cicero *e, cicero_callbacks callbacks)
{
    memset(e->vals, EMPTY, sizeof(e->vals));
    memset(e->vchk, 0xffu, sizeof(e->vchk));
    memset(e->hchk, 0xffu, sizeof(e->hchk));
    memset(e->asqs, 0x00u, sizeof(e->asqs));
    setasq(e->asqs, SQ_H8);
    e->cb = callbacks;

    memcpy(&e->dlsq[0], &double_letter_squares[0], sizeof(e->dlsq));
    memcpy(&e->tlsq[0], &triple_letter_squares[0], sizeof(e->tlsq));

    e->double_letter_squares = &e->dlsq[0];
    e->triple_letter_squares = &e->tlsq[0];
    e->double_word_squares   = &double_word_squares[0];
    e->triple_word_squares   = &triple_word_squares[0];
    e->letter_values         = &letter_values[0];
}

void cicero_rack_add_tile(cicero_rack* rack, char tile)
{
    if (tile == CICERO_UNKNOWN_TILE) {
        return;
    }
    assert(('A' <= tile && tile <= 'Z') || tile == CICERO_BLANK_TILE);
    rack->tiles[char_to_rack_tile(tile)]++;
}

int cicero_make_move(cicero *e, const cicero_move *move)
{
    // NOTE(peter): everything in this function is named as if computing
    // the horizontal cross-checks, but it is actually direction agnotistic.
    char buf[17];
    const char *tiles   = move->tiles;
    const int  *squares = move->squares;
    const int   dir     = move->direction;
    const int   ntiles  = move->ntiles;
    const int   hstride = move->direction;
    const int   vstride = flip_dir(move->direction);
    char *vals = e->vals;
    u32  *hchk = dir == HORZ ? e->hchk : e->vchk;
    u32  *vchk = dir == HORZ ? e->vchk : e->hchk;
    u64  *asqs = e->asqs;
    dimstart horzstart = dir == HORZ ? &colstart : &rowstart;
    dimstart vertstart = dir == HORZ ? &rowstart : &colstart;
    const int lsq   = squares[0];          // left-most square
    const int rsq   = squares[ntiles - 1]; // right-most square
    const int hstart = horzstart(lsq);
    const int hstop  = hstart + hstride * DIM;
    assert(ntiles > 0);
    assert(squares != NULL);
    assert(tiles != NULL);

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
                cicero_edges edges = e->cb.getedges((void*)e->cb.getedgesdata, buf);
                if (edges.terminal) {
                    chk |= tilemask(tilenum(c));
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
                cicero_edges edges = e->cb.getedges((void*)e->cb.getedgesdata, &buf[1]);
                if (edges.terminal) {
                    chk |= tilemask(tilenum(c));
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
                cicero_edges edges = e->cb.getedges((void*)e->cb.getedgesdata, buf);
                if (edges.terminal) {
                    chk |= tilemask(tilenum(c));
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
                cicero_edges edges = e->cb.getedges((void*)e->cb.getedgesdata, &buf[1]);
                if (edges.terminal) {
                    chk |= tilemask(tilenum(c));
                }
            }
            vchk[after] = chk;
            setasq(asqs, after);
        }
    }

    int score = cicero_score_move(e, move);
    cicero_clear_scores(e, move);
    return score;
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

struct state
{
    const cicero *e;
    const u32    *xchk;
    cicero_rack  *r;
    int           anchor;
    int           start;
    int           stride;
    int           stop;
};
typedef struct state state;

struct string
{
    char* buf;
    int   len;
};
typedef struct string string;

internal int range_contains_square(int start, int stride, int length, int sq)
{
    for (int i = 0; i < length; ++i) {
        if (start + stride * i == sq) {
            return 1;
        }
    }
    return 0;
}

internal void revbuf(char* buf, int length)
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

internal void extend_right(const state* ss, int lsq, int sq, string* word, int right_part_length)
{
    word->buf[word->len] = 0;
    const cicero* e = ss->e;
    int* rack = ss->r->tiles;
    const cicero_edges edges_ = e->cb.getedges((void*)e->cb.getedgesdata, word->buf);
    const char* edges = edges_.edges;
    const int terminal = edges_.terminal;
    const char* vals = e->vals;
    const u32* xchk  = ss->xchk;
    const int anchor = ss->anchor;
    const int start  = ss->start;
    const int stride = ss->stride;
    const int stop   = ss->stop;
    const int nextsq = sq + stride;
    const int rsq    = sq - stride;

    if (sq >= stop || vals[sq] == EMPTY) {
        if (/*right_part_length > 0*/ sq > anchor && terminal) {
            assert(word->buf[word->len] == 0);
            e->cb.onlegal((void*)e->cb.onlegaldata, word->buf, lsq, ss->stride);
        }
        if (sq >= stop) { // hit end of board
            return;
        }
        for (const char* tile = edges; *tile != 0; ++tile) {
            const char tint = tilenum(*tile);
            if (rack[tint] == 0) {               // have tile?
                continue;
            }
            if ((xchk[sq] & tilemask(tint)) == 0) {  // meets cross-check?
                continue;
            }
            rack[tint]--;
            word->buf[word->len++] = *tile; // TODO: hoist out of loop
            word->buf[word->len]   = 0;     // TODO: hoist out of loop
            assert(word->len <= DIM);
            extend_right(ss, lsq, nextsq, word, right_part_length + 1);
            word->len--;                    // TODO: hoist out of loop
            word->buf[word->len] = 0;       // TODO: hoist out of loop
            rack[tint]++;
        }
        // NOTE: need to run a second time with blanks so I check both path of using
        //       the blank vs using the actual tile if I have it
        if (rack[BLANK] > 0) {
            for (const char* tile = edges; *tile != 0; ++tile) {
                const char tint = tilenum(*tile);
                if ((xchk[sq] & tilemask(tint)) == 0) {  // meets cross-check?
                    continue;
                }
                rack[BLANK]--;                  // TODO: hoist out of loop
                word->buf[word->len++] = 'a' + (*tile - 'A'); // TODO: hoist len incr out of loop
                word->buf[word->len]   = 0;     // TODO: hoist out of loop
                assert(word->len <= DIM);
                extend_right(ss, lsq, nextsq, word, right_part_length + 1);
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
            extend_right(ss, lsq, nextsq, word, right_part_length);
            word->len--;
        }
    }
}

// TODO: remove `sq` parameter, can calculate it from sq = anchor - strlen(word->buf) - 1 (see line 278 assertion below)
internal void left_part(const state* ss, int sq, int limit, string* word)
{
    word->buf[word->len] = 0; // TEMP?

    const cicero *e = ss->e;
    const cicero_edges edges_   = e->cb.getedges((void*)e->cb.getedgesdata, word->buf);
    const char *edges    = edges_.edges;
    const int   terminal = edges_.terminal;
    const u32 *xchk   = ss->xchk;
    const int  anchor = ss->anchor;
    const int  start  = ss->start;
    const int  stride = ss->stride;
    const int  stop   = ss->stop;
    int       *rack   = ss->r->tiles;
    assert((((anchor - sq) / stride) - 1) == strlen(word->buf));

    extend_right(ss, sq + stride, anchor, word, /*right_part_length*/0);

    if (limit == 0) {
        return;
    }
    assert(e->vals[sq] == EMPTY);

    for (const char* tile = edges; *tile != 0; ++tile) {
        const char tint = tilenum(*tile);
        if (rack[tint] == 0) {              // have tile?
            continue;
        }
        assert(xchk[sq] == ANYTILE); // see section 3.3.1 Placing Left Parts
        rack[tint]--;
        word->buf[word->len++] = *tile; // TODO: hoist incr of len
        word->buf[word->len]   = 0;     // TODO: hoist this out of loop
        assert(sq >= start);
        left_part(ss, sq - stride, limit - 1, word); // try to expand the left part more to the left
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
            left_part(ss, sq - stride, limit - 1, word); // try to expand the left part more to the left
            word->len--;                    // TODO: hoist out of loop
            word->buf[word->len] = 0;       // TODO: hoist this out of loop
            rack[BLANK]++;                  // TODO: hoist out of loop
        }
    }
}

internal void extend_right_on_existing_left_part(const state* ss, int anchor, string* word)
{
    const cicero *e = ss->e;
    const char *vals = e->vals;
    const int start  = ss->start;
    const int stride = ss->stride;
    const int stop   = ss->stop;
    int sq = anchor - stride;
    assert(start <= sq && sq < stop);
    assert(vals[sq] != EMPTY);
    // TODO: more efficient way to extend backwards? maybe make the buffer (DIM+1)*2 so we can start
    //       in the middle?
    while (start <= sq && vals[sq] != EMPTY) {
        word->buf[word->len++] = to_ext(vals[sq]);
        sq -= stride;
    }
    word->buf[word->len] = 0;
    revbuf(word->buf, word->len);
    extend_right(ss, anchor - stride * word->len, anchor, word, /*right_part_length*/0);
    word->len = 0;
}

void cicero_generate_legal_moves(const cicero *e, cicero_rack rack)
{
    const int dirs[] = { HORZ, VERT };

    // printf("--- BEGIN ENGINE FIND --- rack = %s\n", print_rack(&rack));
    const u64  *asqs = e->asqs;
    const char *vals = e->vals;
    char buf[16];
    string word;
    word.buf = buf;
    word.len = 0;
    word.buf[0] = 0;
    state ss;
    ss.e = e;
    ss.r = &rack;
    for (int i = 0; i < 4; ++i) {
        const int base = 64*i;
        u64 msk = asqs[i];
        while (msk > 0) {
            const int anchor = base + lsb(msk);
            for (int i = 0; i < ASIZE(dirs); ++i) {
                const int stride  = dirs[i];
                const int start = dirs[i] == HORZ ? colstart(anchor) : rowstart(anchor);

                ss.anchor = anchor;
                ss.start  = start;
                ss.stride = stride;
                ss.stop   = start + DIM * stride;
                ss.xchk   = dirs[i] == HORZ ? e->hchk : e->vchk;

                if (anchor - stride >= start && vals[anchor - stride] != EMPTY) {
                    extend_right_on_existing_left_part(&ss, anchor, &word);
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
                    left_part(&ss, anchor - stride, limit, &word);
                }
            }

            msk = clearlsb(msk);
        }
    }
    // printf("--- END ENGINE FIND ---\n");
}
