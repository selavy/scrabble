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

int cicero_legal_move_ex(const cicero *e, const cicero_move *move,
        cicero_is_word is_word, const void* udata)
{
    const char *board = e->vals;
    const int hstride = move->direction;
    const int vstride = flip_dir(move->direction);
    const int ntiles = move->ntiles;
    const char *tiles = move->tiles;
    const int *squares = move->squares;
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;
    const dimstart vstart = move->direction == CICERO_HORZ ? rowstart : colstart;

    if (ntiles == 0) {
        return CICERO_NO_TILES_PLAYED;
    } else if (ntiles > 7) {
        return CICERO_TOO_MANY_TILES;
    }

    int word_long_enough = 0;
    int h8_played = board[SQ_H8] != EMPTY;

    // TODO: why did I size this to 32 instead of 15 or 16?
    char buffer[32];
    {
        int tilenum = 0;
        const int root   = squares[0];
        const int start  = hstart(root);
        const int stride = hstride;
        const int stop   = start + DIM * stride;
        const int first  = findbeg(board, start, stop, stride, root);
        char *p = &buffer[0];
        for (int sq = first; sq < stop; sq += stride) {
            char tile;
            if (board[sq] != EMPTY) {
                tile = board[sq];
            } else if (tilenum == ntiles) {
                break;
            } else if (sq == squares[tilenum]) {
                tile = to_eng(tiles[tilenum++]);
            } else {
                TRACE("empty square: %d", sq);
                return CICERO_MOVE_LEAVES_EMPTY_SQUARES;
            }
            *p++ = to_uppercase(to_ext(tile));
        }
        if (tilenum != ntiles) {
            return CICERO_MOVE_NOT_IN_SAME_DIRECTION;
        }
        *p++ = '\0';
        word_long_enough |= (p - &buffer[0]) > 2;
        if (is_word(udata, &buffer[0]) == 0) {
            TRACE("Invalid cross word: '%s'", &buffer[0]);
            return CICERO_INVALID_ROOT_WORD;
        }
    }

    for (int i = 0; i < ntiles; ++i) {
        const int root   = squares[i];
        const int start  = vstart(root);
        const int stride = vstride;
        const int stop   = start + DIM * stride;
        const int first  = findbeg(board, start, stop, stride, root);
        char *p = &buffer[0];
        h8_played |= squares[i] == SQ_H8;
        for (int sq = first; sq != root; sq += stride) {
            *p++ = to_uppercase(to_ext(board[sq]));
        }
        *p++ = tiles[i];
        for (int sq = root + stride; sq < stop && board[sq] != EMPTY; sq += stride) {
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
