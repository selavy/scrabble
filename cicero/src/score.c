#include "cicero_types.h"
#include "cicero_tables.h"

#include <stdio.h>


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

int cicero_score_move(const cicero *e, const cicero_move *move)
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

int cicero_score_move_fast(cicero *e, const cicero_move *move)
{
    int score = 0;
    char       *board   = e->vals;
    const u16  *hscr    = move->direction == CICERO_HORZ ? e->hscr : e->vscr;
    const int  *squares = move->squares;
    const char *tiles   = move->tiles;
    const int   ntiles  = move->ntiles;
    const int   hstride = move->direction;
    const int   vstride = flip_dir(move->direction);
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;
    // const dimstart vstart = move->direction == CICERO_HORZ ? rowstart : colstart;

    place_tiles(board, move);

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
            printf("%c => %d\n", to_ext(tile), value);
        }

        int word_mult = 1;
        for (int i = 0; i < ntiles; ++i) {
            const char tile   = to_eng(tiles[i]);
            const int  square = squares[i];
            const int  value  = letter_values[tile];
            // TODO: adjust double/triple_letter_squares to be 1 less
            const int  mult = (double_letter_squares[square] * triple_letter_squares[square]) - 1;
            word_score += value * mult;
            word_mult  *= double_word_squares[square] * triple_word_squares[square];
            printf("%c => %d x %d => %d\n", to_ext(tile), value, mult, value * mult);
        }
        score += word_score * word_mult;
        printf("word_mult = %d\n", word_mult);
    }

    // int root_score = 0;
    // int root_multiplier = 1;
    // for (int i = 0; i < ntiles; ++i) {
    //     const int tile   = tiles[i];
    //     const int square = squares[i];
    //     root_score += hscr[square];
    //     root_score += letter_values[tile] * double_letter_squares[square] * triple_letter_squares[square];
    // }

    if (ntiles == 7) {
        score += 50;
    }

    remove_tiles(board, move);

    return score;
}
