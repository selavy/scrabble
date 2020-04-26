#include "cicero_types.h"
#include "cicero_tables.h"


int cicero_score_move(const cicero_movegen *e, const cicero_move *move)
{
    int score = 0;
    const int *double_letter_squares = e->double_letter_squares;
    const int *triple_letter_squares = e->triple_letter_squares;
    const int *double_word_squares   = e->double_word_squares;
    const int *triple_word_squares   = e->triple_word_squares;
    const char *board = e->vals;
    const int hstride = move->direction;
    const int vstride = flip_dir(move->direction);
    const int ntiles = move->ntiles;
    const int *squares = move->squares;
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;
    const dimstart vstart = move->direction == CICERO_HORZ ? rowstart : colstart;

    {
        int word_score = 0;
        int word_mult = 1;
        const int root   = squares[0];
        const int start  = hstart(root);
        const int stride = hstride;
        const int stop   = start + DIM * stride;
        for (int sq = root; sq < stop; sq += stride) {
            const char letter = board[sq];
            assert(letter != EMPTY);
            const int value = e->letter_values[letter];
            const int mult  = double_letter_squares[sq] * triple_letter_squares[sq];
            word_score += value * mult;
            word_mult *= double_word_squares[sq] * triple_word_squares[sq];
        }
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
            int sq = root - stride;
            while (sq >= start && board[sq] != EMPTY) {
                sq -= stride;
            }
            for (; sq < stop; sq += stride) {
                const char letter = board[sq];
                const int  value  = letter_values[letter];
                const int  mult   = double_letter_squares[sq] * triple_letter_squares[sq];
                word_score += value * mult;
            }
            const int word_mult = double_word_squares[root] * triple_word_squares[root];
            score += word_score * word_mult;
        }
    }

    if (ntiles == 7) {
        score += 50;
    }

    return score;
}
