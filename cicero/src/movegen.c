#include "cicero_types.h"

struct string
{
    char buf[16];
    int  len;
};
typedef struct string string;

internal int inclusive_length(int beg, int end, int stride) {
    assert(beg <= end);
    return (end - beg) / stride + 1;
}

#if 0
internal void cicero_clear_scores(cicero *e, const cicero_move *move)
{
    for (int i = 0; i < move->ntiles; ++i) {
        e->double_letter_squares[move->squares[i]] = 1;
        e->triple_letter_squares[move->squares[i]] = 1;
    }
}
#endif

char cicero_tile_on_square(const cicero *e, int square)
{
    assert(0 <= square && square < DIM*DIM);
    return to_ext(e->vals[square]);
}

void cicero_savepos_copy(cicero_savepos *sp, const cicero* e)
{
#define SAVEPOS_SAFE_COPY
#if defined(SAVEPOS_SAFE_COPY)
    memcpy(sp->hscr, e->hscr, sizeof(sp->hscr));
    memcpy(sp->vscr, e->vscr, sizeof(sp->vscr));
    memcpy(sp->hchk, e->hchk, sizeof(sp->hchk));
    memcpy(sp->vchk, e->vchk, sizeof(sp->vchk));
    memcpy(sp->asqs, e->asqs, sizeof(sp->asqs));
#else
    memcpy(sp, e->vals, sizeof(*sp));
#endif
}

internal int calc_cached_score(int start, int stop, int stride,
        int root, const cicero *engine)
{
    const char *vals = engine->vals;
    const int *letter_values = engine->s.letter_values;
    int tiles = 0;
    int partial_score = 0;
    // walk left
    for (int sq = root - stride; sq >= start && vals[sq] != EMPTY; sq -= stride) {
        ++tiles;
        partial_score += letter_values[vals[sq]];
    }
    // walk right
    for (int sq = root + stride; sq < stop   && vals[sq] != EMPTY; sq += stride) {
        ++tiles;
        partial_score += letter_values[vals[sq]];
    }
    return tiles != 0 ? partial_score : NOCROSSTILES;
}

void cicero_undo_move(cicero *e, const cicero_savepos* sp,
        const cicero_move *move)
{
    char* board = e->vals;

    // remove tiles
    const int   ntiles  = move->ntiles;
    const int  *squares = move->squares;
    for (int i = 0; i < ntiles; ++i) {
        const int square = squares[i];
        assert(to_ext(board[square]) == move->tiles[i]);
        board[squares[i]] = EMPTY;
    }

    // restore cached state
#define SAVEPOS_SAFE_RESTORE
#if defined(SAVEPOS_SAFE_RESTORE)
    memcpy(e->hscr, sp->hscr, sizeof(e->hscr));
    memcpy(e->vscr, sp->vscr, sizeof(e->vscr));
    memcpy(e->hchk, sp->hchk, sizeof(e->hchk));
    memcpy(e->vchk, sp->vchk, sizeof(e->vchk));
    memcpy(e->asqs, sp->asqs, sizeof(e->asqs));
#else
    memcpy(e->hscr, sp->hscr, sizeof(*sp));
#endif
}

internal cicero_edges getedges(const cicero* e, const char* root)
{
    return e->cb.getedges((void*)e->cb.getedgesdata, root);
}

internal int build_word(const char* vals, const int start, const int stop, const int stride, const int root, string* word)
{
    assert(start <= root && root < stop);
    assert(vals[root] == EMPTY);

    // find left most square after `root`
    int sq = root - stride;
    while (sq >= start && vals[sq] != EMPTY) {
        sq -= stride;
    }
    sq += stride;
    assert(sq == root || vals[sq] != EMPTY);
    assert((sq - stride) < start || vals[sq - stride] == EMPTY);

    // fill in left part
    word->len = 0;
    for (; sq != root; sq += stride) {
        assert(sq < stop);
        assert(vals[sq] != EMPTY);
        word->buf[word->len++] = to_ext(vals[sq]);
    }

    // skip `root`
    const int rootidx = word->len++;
    assert(sq == root);
    assert(vals[root] == EMPTY);
#ifndef NDEBUG
    word->buf[rootidx] = '?';
#endif
    sq += stride;

    // fill in right part
    for (; sq < stop && vals[sq] != EMPTY; sq += stride) {
        word->buf[word->len++] = to_ext(vals[sq]);
    }
    // TRACE("\tbuild_word(%s) stopping on %s (sq=%d stop=%d vals[sq]='%c')", SQNAME(root), SQNAME(sq), sq, stop, to_ext(vals[sq]));
    word->buf[word->len] = '\0';

    return rootidx;
}

internal u32 calc_xchk(const cicero* e, char* word, int index)
{
    u32 xchk = 0;
    for (char c = 'A'; c <= 'Z'; ++c) {
        word[index] = c;
        cicero_edges edges = getedges(e, word);
        if (getedges(e, word).terminal) {
            xchk |= tilemask(tilenum(c));
        }
    }
    return xchk;
}

int cicero_make_move(cicero *e, cicero_savepos *sp, const cicero_move *move)
{
    // TRACE("applying: %.*s", move->ntiles, move->tiles);

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
    u16  *hscr = dir == HORZ ? e->hscr : e->vscr;
    u16  *vscr = dir == HORZ ? e->vscr : e->hscr;
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

    cicero_savepos_copy(sp, e);

    // update horizontal cross-checks
    for (int tidx = 0; tidx < ntiles; ++tidx) {
        const int stride = vstride;
        const int root   = squares[tidx];
        const char tile  = tiles[tidx];
        const char teng  = to_eng(tile);
        const int start  = vertstart(root);
        const int stop   = start + DIM * stride;
        const int before = findbeg(vals, start, stop, stride, root) - stride;
        const int after  = findend(vals, start, stop, stride, root) + stride;
        assert(vals[root] == EMPTY);
        vals[root] = teng;
#ifdef FOR_TEST_COMPLIANCE
        hchk[root] = 0;
        vchk[root] = 0;
#endif
        if (before >= start) {
            assert(getdim(stride, before) == getdim(stride, root));
            assert(vals[before] == EMPTY);
            string word;
            const int idx = build_word(vals, start, stop, stride, before, &word);
            // TRACE("B HORZ XCHK[%s]: word='%s'", SQNAME(before), word.buf);
            hchk[before]  = calc_xchk(e, word.buf, idx);
            hscr[before]  = calc_cached_score(start, stop, stride, before, e);
            setasq(asqs, before);
        }

        if (after < stop) {
            assert(getdim(stride, after) == getdim(stride, root));
            assert(vals[after] == EMPTY);
            string word;
            const int idx = build_word(vals, start, stop, stride, after, &word);
            // TRACE("A HORZ XCHK[%s]: word='%s'", SQNAME(after), word.buf);
            hchk[after]   = calc_xchk(e, word.buf, idx);
            hscr[after]   = calc_cached_score(start, stop, stride, after, e);
            setasq(asqs, after);
        }

        assert(vals[root] != EMPTY);
        clrasq(asqs, root);
    }

    { // update vertical cross-checks
        const int start  = hstart;
        const int stride = hstride;
        const int stop   = hstop;
        const int before = findbeg(vals, start, stop, stride, lsq) - stride;
        const int after  = findend(vals, start, stop, stride, rsq) + stride;
        assert(getdim(stride, lsq) == getdim(stride, rsq)); // move must be on exactly 1 row or col
        if (before >= start) {
            assert(vals[before] == EMPTY);
            assert(getdim(stride, before) == getdim(stride, lsq));
            assert(getdim(stride, before) == getdim(stride, rsq));
            string word;
            const int idx = build_word(vals, start, stop, stride, before, &word);
            // TRACE("B VERT XCHK[%s]: word='%s'", SQNAME(before), word.buf);
            vchk[before]  = calc_xchk(e, word.buf, idx);
            vscr[before]  = calc_cached_score(start, stop, stride, before, e);
            setasq(asqs, before);
        }
        if (after < stop) {
            assert(vals[after] == EMPTY);
            assert(getdim(stride, after) == getdim(stride, lsq));
            assert(getdim(stride, after) == getdim(stride, rsq));
            string word;
            const int idx = build_word(vals, start, stop, stride, after, &word);
            // TRACE("A VERT XCHK[%s]: word='%s'", SQNAME(after), word.buf);
            vchk[after]   = calc_xchk(e, word.buf, idx);
            vscr[after]   = calc_cached_score(start, stop, stride, after, e);
            setasq(asqs, after);
        }
    }

    int score = cicero_score_move(e, move);

#ifdef FOR_TEST_COMPLIANCE
    for (int tidx = 0; tidx < ntiles; ++tidx) {
        const int root = squares[tidx];
        hscr[root] = 0xffffu;
        vscr[root] = 0xffffu;
    }
#endif

    return score;
}

// TODO: add safety checks that the position makes some sense
void cicero_load_position(cicero* e, char board[225])
{
    char *vals = e->vals;
    u32  *hchk = e->hchk;
    u32  *vchk = e->vchk;
    u16  *hscr = e->hscr;
    u16  *vscr = e->vscr;
    u64  *asqs = e->asqs;

    const int* letter_values = e->s.letter_values;

    // place all the tiles from `board` because need them to re-compute
    // the caches
    for (int sq = 0; sq < 225; ++sq) {
        vals[sq] = board[sq] != CICERO_EMPTY_TILE ? to_eng(board[sq]) : EMPTY;
    }

    memset(hscr, 0xffffffffu, sizeof(e->hscr));
    memset(vscr, 0xffffffffu, sizeof(e->vscr));
    memset(vchk, 0xffffffffu, sizeof(e->vchk));
    memset(hchk, 0xffffffffu, sizeof(e->hchk));
    memset(asqs, 0x00000000u, sizeof(e->asqs));
    for (int sq = 0; sq < 225; ++sq) {
        // TODO: combine these if cases

        // horizontal cross-score
        if (vals[sq] == EMPTY) {
            const int vstride = DIM;
            const int vstart  = rowstart(sq);
            const int vstop   = vstart + vstride*DIM;
            scoreresult result;
            int xscore = 0;
            int tiles  = 0;
            result = scoreleft (e, vstart, vstop, vstride, sq);
            xscore += result.score;
            tiles  += result.tiles;
            result = scoreright(e, vstart, vstop, vstride, sq);
            xscore += result.score;
            tiles  += result.tiles;
            // TODO: this isn't going to work if crossing with a blank tile
            hscr[sq] = tiles != 0 ? xscore : 0xffff;
        }

        // vertical cross-score
        if (vals[sq] == EMPTY) {
            const int hstride = 1;
            const int hstart  = colstart(sq);
            const int hstop   = hstart + hstride*DIM;
            scoreresult result;
            int xscore = 0;
            int tiles  = 0;
            result = scoreleft (e, hstart, hstop, hstride, sq);
            xscore += result.score;
            tiles  += result.tiles;
            result = scoreright(e, hstart, hstop, hstride, sq);
            xscore += result.score;
            tiles  += result.tiles;
            // TODO: this isn't going to work if crossing with a blank tile
            vscr[sq] = tiles != 0 ? xscore : 0xffff;
        }

        string word;
        word.len = 0;
        int neighbors = 0;

        // horizontal cross-check
        if (vals[sq] == EMPTY) {
            word.len = 0;
            const int vstride = DIM;
            const int vstart  = rowstart(sq);
            const int vstop   = vstart + vstride*DIM;
            const int beg = findbeg(vals, vstart, vstop, vstride, sq);
            int ss = beg;
            for (; vals[ss] != EMPTY; ss += vstride) {
                assert(ss < sq);
                word.buf[word.len++] = to_ext(vals[ss]);
            }
            // skip `sq`
            const int index = word.len++;
            ss += vstride;
            for (; ss < vstop && vals[ss] != EMPTY; ss += vstride) {
                word.buf[word.len++] = to_ext(vals[ss]);
            }


            // only set if touching other tiles -- this just makes the first
            // move easier to check if all squares are initially set to
            // 0xffffffffu
            if (word.len != 1) {
                word.buf[word.len] = '\0';
                u32 xchk = 0;
                for (char c = 'A'; c <= 'Z'; ++c) {
                    word.buf[index] = c;
                    cicero_edges edges = e->cb.getedges((void*)e->cb.getedgesdata, word.buf);
                    if (edges.terminal) {
                        xchk |= tilemask(tilenum(c));
                    }
                }
                hchk[sq] = xchk;
                setasq(asqs, sq);
            }
        }

        // vertical cross-check
        if (vals[sq] == EMPTY) {
            word.len = 0;
            const int hstride = 1;
            const int hstart  = colstart(sq);
            const int hstop   = hstart + hstride*DIM;
            const int beg = findbeg(vals, hstart, hstop, hstride, sq);
            int ss = beg;
            for (; vals[ss] != EMPTY; ss += hstride) {
                assert(ss < sq);
                word.buf[word.len++] = to_ext(vals[ss]);
            }
            // skip `sq`
            const int index = word.len++;
            ss += hstride;
            for (; ss < hstop && vals[ss] != EMPTY; ss += hstride) {
                word.buf[word.len++] = to_ext(vals[ss]);
            }

            // only set if touching other tiles -- this just makes the first
            // move easier to check if all squares are initially set to
            // 0xffffffffu
            if (word.len != 1) {
                word.buf[word.len] = '\0';
                u32 xchk = 0;
                for (char c = 'A'; c <= 'Z'; ++c) {
                    word.buf[index] = c;
                    cicero_edges edges = e->cb.getedges((void*)e->cb.getedgesdata, word.buf);
                    if (edges.terminal) {
                        xchk |= tilemask(tilenum(c));
                    }
                }
                vchk[sq] = xchk;
                setasq(asqs, sq);
            }
        }

        if (vals[sq] != EMPTY) {
            hchk[sq] = 0;
            vchk[sq] = 0;
        }
    }

    // edge case if is starting position then need to set H8
    if (asqs[0] == 0 && asqs[1] == 0 && asqs[2] == 0 && asqs[3] == 0) {
        setasq(asqs, SQ_H8);
    }
}

cicero_api void cicero_load_position_ex(cicero* e, const cicero* position)
{
    char board[225];
    for (int sq = 0; sq < 225; ++sq) {
        board[sq] = cicero_tile_on_square(position, sq);
    }
    cicero_load_position(e, board);
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
    const u64  *asqs = e->asqs;
    const char *vals = e->vals;
    string word;
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
}
