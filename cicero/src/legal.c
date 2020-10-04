#include "cicero_types.h"

const char *cicero_legal_move_errnum_to_string(int errnum)
{
    switch (errnum) {
        case CICERO_LEGAL_MOVE:                 return "legal move";
        case CICERO_MOVE_NOT_IN_SAME_DIRECTION: return "move not in same direction";
        case CICERO_MOVE_LEAVES_EMPTY_SQUARES:  return "move leaves empty squares";
        case CICERO_INVALID_ROOT_WORD:          return "invalid root word";
        case CICERO_INVALID_CROSS_WORD:         return "invalid cross word";
        case CICERO_NO_TILES_PLAYED:            return "no tiles played";
        case CICERO_TOO_MANY_TILES:             return "too many tiles played";
        case CICERO_WORD_TOO_SHORT:             return "word too short";
        case CICERO_FIRST_MOVE_MUST_OCCUPY_H8:  return "first move must occupy H8";
    }
    return "unknown error code";
}

internal int count_tiles(const cicero_move2* move)
{
    int ntiles = 0;
    for (const char* tile = move->tiles; *tile; ++tile) {
        ntiles += *tile != '.' ? 1 : 0;
    }
    return ntiles;
}

int cicero_legal_move_ex(const cicero *e, const cicero_move *move,
        cicero_is_word is_word, const void* udata)
{
    cicero_move2 m = cicero_make_move2(e, move);
    return cicero_legal_move_ex2(e, &m, is_word, udata);
}

int cicero_legal_move_ex2(const cicero *e, const cicero_move2 *move,
        cicero_is_word is_word, const void* udata)
{
    // Tiles need to be contiguous
    // Tiles must all be on a row or column (same direction)
    // Valid root word
    // Cross words valid
    // Need to have created a word with at least 2 squares

    const char *board = e->vals;
    const int hstride = move->direction;
    const int vstride = flip_dir(move->direction);
    const int ntiles = count_tiles(move);
    const char *tiles = move->tiles;
    const int root = move->square;
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;
    const dimstart vstart = move->direction == CICERO_HORZ ? rowstart : colstart;

    if (ntiles == 0) {
        return CICERO_NO_TILES_PLAYED;
    } else if (ntiles > 7) {
        return CICERO_TOO_MANY_TILES;
    }

    int word_long_enough = 0;
    int h8_played = board[SQ_H8] != EMPTY;

    { // Verify that all tiles that != '.' are empty, and the opposite
        const int start  = hstart(root);
        const int stride = hstride;
        const int stop   = start + DIM * stride;
        int sq = root;
        for (const char* tile = tiles; *tile; ++tile, sq += stride) {
            if (!(sq < stop)) {
                return CICERO_MOVE_NOT_IN_SAME_DIRECTION;
            }
            if (*tile != '.') {
                if (board[sq] != EMPTY) {
                    return CICERO_SQUARE_OCCUPIED;
                }
            } else {
                if (board[sq] == EMPTY) {
                    return CICERO_MOVE_LEAVES_EMPTY_SQUARES;
                }
            }
        }
    }

    char buffer[16];
    { // Verify root word
        char* p = &buffer[0];
        const int start = hstart(root);
        const int stride = hstride;
        const int stop = start + DIM * stride;
        const int first = findbeg(board, start, stop, stride, root);
        for (int sq = first; sq < root; sq += stride) {
            assert(board[sq] != EMPTY);
            *p++ = to_uppercase(to_ext(board[sq]));
        }

        int sq = root;
        for (const char *tilep = tiles; *tilep; ++tilep, sq += stride) {
            assert(sq < stop);
            const int tile = *tilep != '.' ? to_eng(*tilep) : board[sq];
            *p++ = to_uppercase(to_ext(tile));
        }

        for (; sq < stop && board[sq] != EMPTY; ++sq) {
            *p++ = to_uppercase(to_ext(board[sq]));
        }

        *p++ = '\0';
        word_long_enough |= (p - &buffer[0]) > 2;
        if (is_word(udata, &buffer[0]) == 0) {
            TRACE("Invalid cross word: '%s'", &buffer[0]);
            return CICERO_INVALID_ROOT_WORD;
        }
    }

    int root_sq = root;
    for (const char* tilep = tiles; *tilep; ++tilep, root_sq += hstride) {
        const int start  = vstart(root_sq);
        const int stride = vstride;
        const int stop   = start + DIM * stride;
        const int first  = findbeg(board, start, stop, stride, root_sq);
        char *p = &buffer[0];
        h8_played |= root_sq == SQ_H8;
        for (int sq = first; sq != root_sq; sq += stride) {
            *p++ = to_uppercase(to_ext(board[sq]));
        }
        *p++ = *tilep;
        for (int sq = root_sq + stride; sq < stop && board[sq] != EMPTY; sq += stride) {
            *p++ = to_uppercase(to_ext(board[sq]));
        }
        *p++ = '\0';
        word_long_enough |= (p - &buffer[0]) > 2;
        if ((p - &buffer[0]) > 2 && is_word(udata, &buffer[0]) == 0) {
            TRACE("Invalid cross word: '%s'", &buffer[0]);
            return CICERO_INVALID_CROSS_WORD;
        }
    }

    if (!word_long_enough) {
        return CICERO_WORD_TOO_SHORT;
    }
    if (!h8_played) {
        return CICERO_FIRST_MOVE_MUST_OCCUPY_H8;
    }

    return CICERO_LEGAL_MOVE;
}

internal int is_word_using_getedges(const void *data, const char *word)
{
    const cicero *e = data;
    return e->cb.getedges((void*)e->cb.getedgesdata, word).terminal;
}

int cicero_legal_move(const cicero *e, const cicero_move *move)
{
    return cicero_legal_move_ex(e, move, &is_word_using_getedges, e);
}
