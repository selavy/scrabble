#ifndef GENERAL_H_
#define GENERAL_H_

#define _DEBUG

// 26 letters + blank
#define BLANK 26
#define NUM_LETTERS 27
#define MAX_PLAYERS 4
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
#define BLANK_BIT (1 << 7)
#define IS_BLANK(x) (((x) & BLANK_BIT) != 0)
#define MAKE_BLANK(x) do { (x) |= BLANK_BIT; } while(0)
#define GET_LETTER(x) (x & ~BLANK_BIT)

#define PRT_EOL printf("\n")
#define PRT_TAB printf("\t")
#define TILE_TO_CHAR(letter) ((IS_BLANK(letter)) ? '*' : (((letter) >= EMPTY) ? ' ' : 'A' + (letter)))
#define TO_CHAR(letter) ('A' + (letter))
#define CHAR_TO_TILE(c) (((c) == '*') ? BLANK_BIT : ((c) - 'A'))
#define ROW_TO_BOARD(row) (BOARD_SIZE - (row))
#define COL_TO_BOARD(col) ((col) - 1)

typedef unsigned char tile_t;
typedef unsigned long score_t;
typedef unsigned char turn_t;
typedef unsigned char frequency_t[NUM_LETTERS];

#endif
