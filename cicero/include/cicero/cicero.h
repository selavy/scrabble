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

typedef void         (*on_legal_move)(void *data, const char* word, int sq, int dir);
typedef cicero_edges (*prefix_edges )(void *data, const char* prefix);

// TODO: switch to this?
// struct cicero_move
// {
//     cicero_direction direction;
//     int              square;
//     const char      *word;
// };
struct cicero_move
{
    const char      *tiles;      // A-Z=normal tiles, a-z=blank tiles
    const int       *squares;    // 0 indexed
    int              ntiles;
    cicero_direction direction;
};
typedef struct cicero_move cicero_move;

enum cicero_special_tiles
{
    CICERO_BLANK_TILE   = '?',
    CICERO_UNKNOWN_TILE = ' ',
    CICERO_EMPTY_TILE   = ' ',
};

struct cicero_rack
{
    int tiles[27];
};
typedef struct cicero_rack cicero_rack;

// note: cicero will not touch the user data, however it is passed back to the user as non-const.
struct cicero_callbacks
{
    on_legal_move onlegal;
    const void*   onlegaldata;
    prefix_edges  getedges;
    const void*   getedgesdata;
};
typedef struct cicero_callbacks cicero_callbacks;

struct cicero_savepos
{
    // TODO: see if I can reduce how much state is saved
    char     vals[225];
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

    char     vals[225];
    uint16_t hscr[225]; // if playing horizontally at square, how many additional points you'd score
    uint16_t vscr[225];
    uint32_t hchk[225]; // if playing horizontally, need to check hchk
    uint32_t vchk[225];
    uint64_t asqs[4];

    cicero_callbacks cb;
    // TODO: pull out of cicero struct so only need 1 copy
    cicero_savepos   sp;

    const int *double_letter_squares;
    const int *triple_letter_squares;
    const int *double_word_squares;
    const int *triple_word_squares;
    const int *letter_values;
};
typedef struct cicero cicero;

// -------------------------------------------------------------------------- //
// Public API
// -------------------------------------------------------------------------- //


// A-Z = regular tile, a-z = blank, ' ' = empty
cicero_api char cicero_tile_on_square(const cicero *e, int square);

cicero_api void cicero_make_rack(cicero_rack *rack, const char *const tiles);

// precondition: 'A' <= tile <= 'Z' or tile == ' '
cicero_api void cicero_rack_add_tile(cicero_rack *rack, char tile);

// note: no memory is allocated to intialize `cicero`
cicero_api void cicero_init(cicero *e, cicero_callbacks callbacks);

// precondition: move is legal and valid
cicero_api int  cicero_make_move(cicero *e, const cicero_move *move);

// precondition: `move` was just played via cicero_make_move() and `sp`
// was passed to corresponding call to cicero_make_move
cicero_api void cicero_undo_move(cicero *e, const cicero_savepos* sp,
        const cicero_move *move);

// will call `onlegal` callback with all legal moves from `rack`
cicero_api void cicero_generate_legal_moves(const cicero *e, cicero_rack rack);

// precondition: `move` was just played
cicero_api int  cicero_score_move(const cicero *e, const cicero_move *move);


// -------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif // CICERO__H_
