#include "cicero_types.h"
#include "cicero_tables.h"


void cicero_init_ex(cicero *e, cicero_callbacks callbacks,
        cicero_scoring scoring)
{
    memset(e->vals, EMPTY, sizeof(e->vals));
    memset(e->hscr, 0xffu, sizeof(e->hscr));
    memset(e->vscr, 0xffu, sizeof(e->vscr));
    memset(e->vchk, 0xffu, sizeof(e->vchk));
    memset(e->hchk, 0xffu, sizeof(e->hchk));
    memset(e->asqs, 0x00u, sizeof(e->asqs));
    setasq(e->asqs, SQ_H8);
    e->cb = callbacks;

    e->s.double_letter_squares = scoring.double_letter_squares ?
        scoring.double_letter_squares : &scrabble__double_letter_squares[0];
    e->s.triple_letter_squares = scoring.triple_letter_squares ?
        scoring.triple_letter_squares : &scrabble__triple_letter_squares[0];
    e->s.double_word_squares = scoring.double_word_squares ?
        scoring.double_word_squares : &scrabble__double_word_squares[0];
    e->s.triple_word_squares = scoring.triple_word_squares ?
        scoring.triple_word_squares : &scrabble__triple_word_squares[0];
    e->s.letter_values = scoring.letter_values ?
        scoring.letter_values : &scrabble__letter_values[0];
    e->s.bingo_bonus = scoring.bingo_bonus;
}

void cicero_init(cicero *e, cicero_callbacks callbacks)
{
    cicero_scoring defaults;
    defaults.double_letter_squares = NULL;
    defaults.triple_letter_squares = NULL;
    defaults.double_word_squares   = NULL;
    defaults.triple_word_squares   = NULL;
    defaults.letter_values         = NULL;
    defaults.bingo_bonus           = 50;
    cicero_init_ex(e, callbacks, defaults);
}

void cicero_init_wwf(cicero *e, cicero_callbacks callbacks)
{
    cicero_scoring defaults;
    defaults.double_letter_squares = words_with_friends__double_letter_squares;
    defaults.triple_letter_squares = words_with_friends__triple_letter_squares;
    defaults.double_word_squares   = words_with_friends__double_word_squares;
    defaults.triple_word_squares   = words_with_friends__triple_word_squares;
    defaults.letter_values         = words_with_friends__letter_values;
    defaults.bingo_bonus           = 35;
    cicero_init_ex(e, callbacks, defaults);
}

