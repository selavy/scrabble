#include "cicero_types.h"

#if 0
internal int get_word_score(const int *squares, int nsquares, const cicero *e)
{
    const int *double_word_squares = e->double_word_squares;
    const int *triple_word_squares = e->triple_word_squares;
    int multiplier = 1;
    for (int i = 0; i < nsquares; ++i) {
        multiplier *= double_word_squares[squares[i]];
        multiplier *= triple_word_squares[squares[i]];
    }
    return multiplier;
}
#endif

#if 0
int cicero_score_move_slow(const cicero *e, const cicero_move *move)
{
    int score = 0;
    const int *double_letter_squares = e->double_letter_squares;
    const int *triple_letter_squares = e->triple_letter_squares;
    const int *double_word_squares   = e->double_word_squares;
    const int *triple_word_squares   = e->triple_word_squares;
    const int *letter_values         = e->letter_values;
    const char *board = e->vals;
    const int hstride = move->direction;
    const int vstride = flip_dir(move->direction);
    const int ntiles = move->ntiles;
    const int *squares = move->squares;
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;
    const dimstart vstart = move->direction == CICERO_HORZ ? rowstart : colstart;

    {
        int word_score = 0;
        const int root   = squares[0];
        const int start  = hstart(root);
        const int stride = hstride;
        const int stop   = start + DIM * stride;
        const int first  = findbeg(board, start, stop, stride, root);
        for (int sq = first; sq < stop && board[sq] != EMPTY; sq += stride) {
            const char tile = board[sq];
            const int value = letter_values[tile];
            const int mult  = double_letter_squares[sq] * triple_letter_squares[sq];
            word_score += value * mult;
        }
        const int word_mult = get_word_score(squares, ntiles, e);
        score += word_score * word_mult;
    }

    {
        int word_score = 0;
        for (int i = 0; i < ntiles; ++i) {
            int word_score = 0;
            const int root   = squares[i];
            const int start  = vstart(root);
            const int stride = vstride;
            const int stop   = start + DIM * stride;
            const int first  = findbeg(board, start, stop, stride, root);
            int tiles = 0;
            for (int sq = first; sq < stop && board[sq] != EMPTY; sq += stride, ++tiles) {
                const char tile = board[sq];
                const int value = letter_values[tile];
                const int mult  = double_letter_squares[sq] * triple_letter_squares[sq];
                word_score += value * mult;
            }
            if (tiles > 1) {
                const int word_mult = double_word_squares[root] * triple_word_squares[root];
                score += word_score * word_mult;
            }
        }
    }

    if (ntiles == 7) {
        score += 50;
    }

    return score;
}
#endif

// TODO: remove
internal void place_tiles(char *board, const cicero_move *move)
{
    const int   ntiles  = move->ntiles;
    const int  *squares = move->squares;
    const char *tiles   = move->tiles;
    for (int i = 0; i < ntiles; ++i) {
        const int square = squares[i];
        const int tile   = tiles[i];
        assert(board[square] == EMPTY);
        board[square] = to_eng(tile);
    }
}

// TODO: remove
internal void remove_tiles(char *board, const cicero_move *move)
{
    const int   ntiles  = move->ntiles;
    const int  *squares = move->squares;
    for (int i = 0; i < ntiles; ++i) {
        const int square = squares[i];
        assert(to_ext(board[square]) == move->tiles[i]);
        board[squares[i]] = EMPTY;
    }
}

int cicero_score_move(const cicero *e, const cicero_move *move)
{
    const char *board  = e->vals;
    const char *tiles  = move->tiles;
    const u16 *xscr    = move->direction == CICERO_HORZ ? e->hscr : e->vscr;
    const int *squares = move->squares;
    const int  ntiles  = move->ntiles;
    const int  hstride = move->direction;
    const int  vstride = flip_dir(move->direction);
    const int *dlsqs   = e->s.double_letter_squares;
    const int *tlsqs   = e->s.triple_letter_squares;
    const int *dwsqs   = e->s.double_word_squares;
    const int *twsqs   = e->s.triple_word_squares;
    const int *letter_values = e->s.letter_values;
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;

    int root_score  = 0;
    int cross_score = 0;
    const int bingo_bonus = ntiles == 7 ? e->s.bingo_bonus : 0;

    {
        int word_score = 0;
        const int root   = squares[0];
        const int start  = hstart(root);
        const int stride = hstride;
        const int stop   = start + DIM * stride;
        const int first  = findbeg(board, start, stop, stride, root);
        for (int sq = first; sq < stop && board[sq] != EMPTY; sq += stride) {
            const char tile = board[sq];
            const int value = letter_values[tile];
            word_score += value;
        }

        int word_mult = 1;
        for (int i = 0; i < ntiles; ++i) {
            const char tile   = to_eng(tiles[i]);
            const int  square = squares[i];
            const int  value  = letter_values[tile];
            // TODO: adjust double/tlsqs to be 1 less
            const int  mult = dlsqs[square] * tlsqs[square];
            word_score += value * (mult - 1);
            word_mult  *= dwsqs[square] * twsqs[square];
        }

        root_score = word_score * word_mult;
    }

    // TODO: move into other loop through squares played
    for (int i = 0; i < ntiles; ++i) {
        const int  square = squares[i];
        const char teng   = to_eng(tiles[i]);
        // NOTE: have to use a sentinel value because a blank is worth 0
        // and still get the double score for the played tile (and multiplier)
        // in that case.
        if (xscr[square] != NOCROSSTILES) {
            const int word_mult = dwsqs[square] * twsqs[square];
            const int letter_value = letter_values[teng];
            const int letter_mult  = dlsqs[square] * tlsqs[square];
            const int value        = letter_value * letter_mult;
            const int total        = (xscr[square] + value) * word_mult;
            cross_score += total;
        }
    }

    return root_score + cross_score + bingo_bonus;
}

cicero_multiplier cicero_square_multiplier(const cicero* e, int sq)
{
    if (!(0 <= sq && sq < 225)) {
        return CICERO_NORMAL_SQ;
    }

    if (e->s.double_letter_squares[sq] != 1) {
        return CICERO_DOUBLE_LETTER_SQ;
    }
    if (e->s.triple_letter_squares[sq] != 1) {
        return CICERO_TRIPLE_LETTER_SQ;
    }
    if (e->s.double_word_squares[sq] != 1) {
        return CICERO_DOUBLE_WORD_SQ;
    }
    if (e->s.triple_word_squares[sq] != 1) {
        return CICERO_TRIPLE_WORD_SQ;
    }
    return CICERO_NORMAL_SQ;
}
