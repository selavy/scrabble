#pragma once

#include <cstdint>
#include <array>

// clang-format off

// edges in ['A', 'Z'] with 0 marking final
struct Edges { bool terminal; char edges[27]; }; // TODO: better name

// TODO(peter): don't need rsq
typedef void  (*OnLegalMove)(void* data, const char* word, int lsq, int rsq, int dir);
typedef Edges (*PrefixEdges)(void* data, const char* prefix);

struct Engine
{
    char     vals[225]; // TODO(peter): maybe remove and pass in from UI?
    uint32_t hchk[225];
    uint32_t vchk[225];
    uint64_t asqs[4];

    OnLegalMove on_legal_move;
    void*       on_legal_move_data;
    PrefixEdges prefix_edges;
    void*       prefix_edges_data;
};

struct EngineMove
{
    const char* tiles;
    const int*  squares;
    int ntiles;
    int direction;
};

struct EngineRack
{
    int tiles[27] = { 0 }; // 27 == BLANK
};
// clang-format on

// must set on_legal_move, get_prefix_edges and corresponding data pointers
// yourself
extern void engine_init(Engine* e);

// tiles from [a-zA-Z] where lowercase = blank tile
// precondition: `squares` is sorted
extern void engine_make_move(Engine* e, const EngineMove* move);

extern void engine_find(const Engine* e, EngineRack rack);

//-----------------------------------------------------------------------------
// TEMP TEMP -- utility functions to delete
//-----------------------------------------------------------------------------

// 0 = meets all xchecks, >0 is the first square that violated
extern int engine_xchk(const Engine* e, const EngineMove* m); // TEMP TEMP

extern void engine_print_anchors(const Engine* e);

extern const char* SQ_(int sq) noexcept;
