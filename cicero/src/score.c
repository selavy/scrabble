#include "cicero_types.h"
#include "cicero_tables.h"

#include <stdio.h>


int get_word_score(const int *squares, int nsquares, const cicero_movegen *e)
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

int cicero_score_move(const cicero_movegen *e, const cicero_move *move)
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


void cicero_clear_scores(cicero_movegen *m, const cicero_move *move)
{
    for (int i = 0; i < move->ntiles; ++i) {
        m->double_letter_squares[move->squares[i]] = 1;
        m->triple_letter_squares[move->squares[i]] = 1;
    }
}
