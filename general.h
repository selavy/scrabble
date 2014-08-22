#ifndef GENERAL_H_
#define GENERAL_H_

#define _DEBUG

// 26 letters + a blank
#define NUM_LETTERS 27
#define MAX_PLAYERS 4
#define BLANK (NUM_LETTERS - 1)
#define EMPTY (NUM_LETTERS)
#define DOUBLE_LETTER (NUM_LETTERS + 1)
#define TRIPLE_LETTER (NUM_LETTERS + 2)
#define DOUBLE_WORD (NUM_LETTERS + 3)
#define TRIPLE_WORD (NUM_LETTERS + 4)
#define MIDDLE_SQUARE (NUM_LETTERS + 5)
#define SUCCESS 0
#define FAILURE -1
#define ERROR -2
#define TRUE 1
#define FALSE 0

#define PRT_EOL printf("\n")
#define PRT_TAB printf("\t")
#define TILE_TO_CHAR(letter) (((letter) == BLANK) ? '*' : (((letter) > BLANK) ? ' ' : 'A' + (letter)))
#define CHAR_TO_TILE(c) (((c) == '*') ? BLANK : ((c) - 'A'))
#define ROW_TO_BOARD(row) (BOARD_SIZE - (row))
#define COL_TO_BOARD(col) ((col) - 1)

typedef signed char tile_t;
typedef unsigned long score_t;
typedef unsigned char turn_t;
typedef unsigned char frequency_t[NUM_LETTERS];

#endif
