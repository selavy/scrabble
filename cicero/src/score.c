#include "cicero_types.h"

int cicero_score_move(const cicero *e, const cicero_move *move)
{
    cicero_move2 m = cicero_make_move2(e, move);
#if 0
    TRACE("move:");
    for (int i = 0; i < move->ntiles; ++i) {
        TRACE("\t%d = %c", move->squares[i], move->tiles[i]);
    }
    TRACE("\tDIRECTION = %d", move->direction);

    TRACE("move2:");
    int sq = m.square;
    int stride = m.direction;
    for (const char *tile = m.tiles; *tile; ++tile, sq += stride) {
        int val = *tile == '.' ? 0 : e->s.letter_values[to_eng(*tile)];
        TRACE("\t%d = %c (%d)", sq, *tile, val);
    }
    TRACE("\tDIRECTION = %d", m.direction);
#endif
    return cicero_score_move2(e, &m);
}

int cicero_score_move2(const cicero *e, const cicero_move2 *move)
{
    const char *board   = e->vals;
    const char *tiles   = move->tiles;
    const u16  *xscr    = move->direction == CICERO_HORZ ? e->hscr : e->vscr;
    const int   root    = move->square;
    const int   stride = move->direction;
    const int  *dlsqs   = e->s.double_letter_squares;
    const int  *tlsqs   = e->s.triple_letter_squares;
    const int  *dwsqs   = e->s.double_word_squares;
    const int  *twsqs   = e->s.triple_word_squares;
    const int  *letter_values = e->s.letter_values;
    const dimstart hstart = move->direction == CICERO_HORZ ? colstart : rowstart;

    int word_score = 0;
    const int start  = hstart(root);
    const int stop   = start + DIM * stride;
    const int first  = findbeg(board, start, stop, stride, root);
    for (int sq = first; sq < stop && board[sq] != EMPTY; sq += stride) {
        word_score += letter_values[board[sq]];
    }

    int root_score  = 0;
    int cross_score = 0;
    int ntiles = 0;
    int word_mult = 1;
    int square = root;
    for (const char *tilep = tiles; *tilep; ++tilep, square += stride) {
        if (*tilep == '.') {
            continue;
        }
        ++ntiles;
        const char tile   = to_eng(*tilep);
        const int  letter_value  = letter_values[tile];
        // TODO: adjust double/tlsqs to be 1 less
        const int  letter_mult = dlsqs[square] * tlsqs[square];
        word_score += letter_value * (letter_mult - 1);
        word_mult  *= dwsqs[square] * twsqs[square];

        // NOTE: have to use a sentinel value because a blank is worth 0
        // and still get the double score for the played tile (and multiplier)
        // in that case.
        if (xscr[square] != NOCROSSTILES) {
            const int xword_mult  = dwsqs[square] * twsqs[square];
            const int cross_value = letter_value * letter_mult;
            cross_score += (xscr[square] + cross_value) * xword_mult;
        }
    }

    root_score = word_score * word_mult;

    const int bingo_bonus = ntiles == 7 ? e->s.bingo_bonus : 0;
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
