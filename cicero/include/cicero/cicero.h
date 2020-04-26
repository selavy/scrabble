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

// TODO: switch to this:
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

struct cicero
{
    char     vals[225];
    uint32_t hchk[225];
    uint32_t vchk[225];
    uint64_t asqs[4];
    cicero_callbacks cb;

    int        dlsq[225]; // TODO: remove
    int        tlsq[225]; // TODO: remove
    int       *double_letter_squares;
    int       *triple_letter_squares;
    const int *double_word_squares;
    const int *triple_word_squares;
    const int *letter_values;
};
typedef struct cicero cicero;


// A-Z = regular tile, a-z = blank, ' ' = empty
cicero_api char cicero_tile_on_square(const cicero *e, int square);

// precondition: 'A' <= tile <= 'Z' or tile == ' '
cicero_api void cicero_rack_add_tile(cicero_rack* rack, char tile);

// note: no memory is allocated to intialize `cicero`
cicero_api void cicero_init(cicero *e, cicero_callbacks callbacks);

// precondition: move is legal and valid
cicero_api int  cicero_make_move(cicero *e, const cicero_move *move);

// will call `onlegal` callback with all legal moves from `rack`
cicero_api void cicero_generate_legal_moves(const cicero *e, cicero_rack rack);

// precondition: `move` was just played
cicero_api int  cicero_score_move(const cicero *e, const cicero_move *move);

// TEMP TEMP
cicero_api void cicero_clear_scores(cicero *e, const cicero_move *move);

#ifdef __cplusplus
}
#endif

#endif // CICERO__H_
