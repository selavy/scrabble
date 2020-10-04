#ifndef CICERO__H_
#define CICERO__H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define cicero_api extern

enum cicero_direction
{
    CICERO_HORZ =  1,
    CICERO_VERT = 15,
};
typedef enum cicero_direction cicero_direction;

struct cicero_edges
{
    int  terminal;
    char edges[27]; // [A-Z], null-terminated
};
typedef struct cicero_edges cicero_edges;

// TODO: prefix these typedefs
typedef void (*on_legal_move)(void *data, const char *word, int sq, int dir);
typedef cicero_edges (*prefix_edges)(void *data, const char *prefix);
// non-zero indicates is valid word
typedef int (*cicero_is_word)(const void *data, const char *word);

enum cicero_legal_move_errnum
{
    CICERO_LEGAL_MOVE                 =  0,
    CICERO_MOVE_NOT_IN_SAME_DIRECTION = -1,
    CICERO_MOVE_LEAVES_EMPTY_SQUARES  = -2,
    CICERO_INVALID_ROOT_WORD          = -3,
    CICERO_INVALID_CROSS_WORD         = -4,
    CICERO_NO_TILES_PLAYED            = -5,
    CICERO_TOO_MANY_TILES             = -6,
    CICERO_WORD_TOO_SHORT             = -7,
    CICERO_FIRST_MOVE_MUST_OCCUPY_H8  = -8,
};

enum cicero_multiplier_
{
    CICERO_NORMAL_SQ,
    CICERO_DOUBLE_LETTER_SQ,
    CICERO_TRIPLE_LETTER_SQ,
    CICERO_DOUBLE_WORD_SQ,
    CICERO_TRIPLE_WORD_SQ,
};
typedef enum cicero_multiplier_ cicero_multiplier;

// TODO: switch to:
struct cicero_move2
{
    char tiles[16]; // use '.' to represent tiles that weren't played
    int  square;
    int  direction;
};
typedef struct cicero_move2 cicero_move2;

struct cicero_move
{
    const char      *tiles;    // A-Z=normal tiles, a-z=blank tiles
    const int       *squares;  // 0 indexed
    int              ntiles;
    cicero_direction direction;
};
typedef struct cicero_move cicero_move;

enum cicero_external_tile
{
    CICERO_TILE_A       = 'A',
    CICERO_TILE_B       = 'B',
    CICERO_TILE_C       = 'C',
    CICERO_TILE_D       = 'D',
    CICERO_TILE_E       = 'E',
    CICERO_TILE_F       = 'F',
    CICERO_TILE_G       = 'G',
    CICERO_TILE_H       = 'H',
    CICERO_TILE_I       = 'I',
    CICERO_TILE_J       = 'J',
    CICERO_TILE_K       = 'K',
    CICERO_TILE_L       = 'L',
    CICERO_TILE_M       = 'M',
    CICERO_TILE_N       = 'N',
    CICERO_TILE_O       = 'O',
    CICERO_TILE_P       = 'P',
    CICERO_TILE_Q       = 'Q',
    CICERO_TILE_R       = 'R',
    CICERO_TILE_S       = 'S',
    CICERO_TILE_T       = 'T',
    CICERO_TILE_U       = 'U',
    CICERO_TILE_V       = 'V',
    CICERO_TILE_W       = 'W',
    CICERO_TILE_X       = 'X',
    CICERO_TILE_Y       = 'Y',
    CICERO_TILE_Z       = 'Z',
    CICERO_TILE_BLANK_A = 'a',
    CICERO_TILE_BLANK_B = 'b',
    CICERO_TILE_BLANK_C = 'c',
    CICERO_TILE_BLANK_D = 'd',
    CICERO_TILE_BLANK_E = 'e',
    CICERO_TILE_BLANK_F = 'f',
    CICERO_TILE_BLANK_G = 'g',
    CICERO_TILE_BLANK_H = 'h',
    CICERO_TILE_BLANK_I = 'i',
    CICERO_TILE_BLANK_J = 'j',
    CICERO_TILE_BLANK_K = 'k',
    CICERO_TILE_BLANK_L = 'l',
    CICERO_TILE_BLANK_M = 'm',
    CICERO_TILE_BLANK_N = 'n',
    CICERO_TILE_BLANK_O = 'o',
    CICERO_TILE_BLANK_P = 'p',
    CICERO_TILE_BLANK_Q = 'q',
    CICERO_TILE_BLANK_R = 'r',
    CICERO_TILE_BLANK_S = 's',
    CICERO_TILE_BLANK_T = 't',
    CICERO_TILE_BLANK_U = 'u',
    CICERO_TILE_BLANK_V = 'v',
    CICERO_TILE_BLANK_W = 'w',
    CICERO_TILE_BLANK_X = 'x',
    CICERO_TILE_BLANK_Y = 'y',
    CICERO_TILE_BLANK_Z = 'z',
    CICERO_TILE_BLANK   = '?',
    CICERO_TILE_UNKNOWN = ' ',
    CICERO_TILE_EMPTY   = ' ',
};

// TODO: need a constant for 26 = BLANKTILEs
struct cicero_rack
{
    int tiles[27];
};
typedef struct cicero_rack cicero_rack;

// note: cicero will not touch the user data, however it is passed back to the user as non-const.
struct cicero_callbacks
{
    on_legal_move onlegal;
    const void   *onlegaldata;
    prefix_edges  getedges;
    const void   *getedgesdata;
};
typedef struct cicero_callbacks cicero_callbacks;

// TODO: document the requirements of this struct
struct cicero_scoring
{
    const int *double_letter_squares;
    const int *triple_letter_squares;
    const int *double_word_squares;
    const int *triple_word_squares;
    const int *letter_values;
    int bingo_bonus;
};
typedef struct cicero_scoring cicero_scoring;

struct cicero_savepos
{
    // TODO: see if I can reduce how much state is saved
    uint16_t hscr[225]; // if playing horizontally at square, how many additional points you'd score
    uint16_t vscr[225];
    uint32_t hchk[225]; // if playing horizontally, need to check hchk
    uint32_t vchk[225];
    uint64_t asqs[4];
};
typedef struct cicero_savepos cicero_savepos;

struct cicero
{
    // https://bestlifeonline.com/highest-scoring-scrabble-move/
    // highest scoring word is "OXYPHENBUTAZONE" for 1,458 points,
    // and highest possible score on 1 move is 1,778 points

    // the tiles on the board, use cicero_tile_on_square to access
    char     vals[225];

    // cached cross-score values. hscr holds the cross-score formed vertically
    // for a tile played as part of a horizontal word.
    uint16_t hscr[225]; // if playing horizontally at square, how many additional points you'd score
    uint16_t vscr[225];

    // cross-checks, if playing horizontally, check hchk to see if words
    // formed are valid vertically
    uint32_t hchk[225];
    uint32_t vchk[225];

    // anchor squares bitmask
    uint64_t asqs[4];

    cicero_callbacks cb;
    cicero_scoring   s;
};
typedef struct cicero cicero;

// -------------------------------------------------------------------------- //
// Public API
// -------------------------------------------------------------------------- //

// A-Z = regular tile, a-z = blank, ' ' = empty
cicero_api char cicero_tile_on_square(const cicero *e, int square);

// TODO: this should return an error code
cicero_api void cicero_make_rack(cicero_rack *rack, const char *const tiles);

// precondition: 'A' <= tile <= 'Z' or tile == ' '
cicero_api void cicero_rack_add_tile(cicero_rack *rack, char tile);

// note: no memory is allocated to intialize `cicero`
// if `scoring` is NULL, then uses default official Scrabble values
cicero_api void cicero_init_ex(cicero *e, cicero_callbacks callbacks,
        cicero_scoring scoring);

cicero_api void cicero_init(cicero *e, cicero_callbacks callbacks);

// words with friends scoring initialization
cicero_api void cicero_init_wwf(cicero *e, cicero_callbacks callbacks);

// precondition: move is legal and valid
cicero_api int  cicero_make_move(cicero *e, cicero_savepos* sp,
        const cicero_move *move);

// precondition: `move` was just played via cicero_make_move() and `sp`
// was passed to corresponding call to cicero_make_move
cicero_api void cicero_undo_move(cicero *e, const cicero_savepos* sp,
        const cicero_move *move);

cicero_api void cicero_undo_move2(cicero *e, const cicero_savepos* sp,
        const cicero_move2 *move);

// will call `onlegal` callback with all legal moves from `rack`
cicero_api void cicero_generate_legal_moves(const cicero *e, cicero_rack rack);

// TODO: maybe this function shouldn't be part of the public api since it is
//       easy to mess up
// precondition: `move` was just played
cicero_api int  cicero_score_move(const cicero *e, const cicero_move *move);

cicero_api int  cicero_score_move2(const cicero *e, const cicero_move2 *move);

cicero_api int  cicero_legal_move_ex(const cicero *e, const cicero_move *move,
        cicero_is_word is_word, const void *udata);

cicero_api int  cicero_legal_move(const cicero *e, const cicero_move *move);

cicero_api const char *cicero_legal_move_errnum_to_string(int errnum);

cicero_api cicero_multiplier cicero_square_multiplier(const cicero* e, int sq);

/* initialize the board from the position in `board` */
cicero_api void cicero_load_position(cicero* e, char board[225]);

cicero_api void cicero_load_position_ex(cicero* e, const cicero* position);

cicero_api cicero_move2 cicero_make_move2(const cicero* e, const cicero_move* m);

// -------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#undef cicero_api

#endif // CICERO__H_
