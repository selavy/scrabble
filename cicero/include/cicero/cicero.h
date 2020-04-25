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

struct cicero_callbacks
{
    on_legal_move onlegal;
    const void*   onlegaldata;
    prefix_edges  getedges;
    const void*   getedgesdata;
};
typedef struct cicero_callbacks cicero_callbacks;

struct cicero_movegen
{
    char     vals[225];
    uint32_t hchk[225];
    uint32_t vchk[225];
    uint64_t asqs[4];
    cicero_callbacks cb;
};
typedef struct cicero_movegen cicero_movegen;


cicero_api void cicero_movegen_init(cicero_movegen *m, cicero_callbacks callbacks);
// precondition: move is legal and valid
cicero_api void cicero_movegen_make_move(cicero_movegen *m, const cicero_move *move);
cicero_api void cicero_movegen_generate(const cicero_movegen *m, cicero_rack rack);
// precondition: 'A' <= tile <= 'Z' or tile == ' '
cicero_api void cicero_rack_add_tile(cicero_rack* rack, char tile);

#ifdef __cplusplus
}
#endif

#endif // CICERO__H_
