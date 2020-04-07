#pragma once

#include <cstdint>
#include <array>


typedef int (*wordchk_t)(void* data, const char* const word);

// clang-format off
struct Engine
{
    char     vals[225]; // TODO(peter): maybe remove and pass in from UI?
    uint32_t hchk[225];
    uint32_t vchk[225];
    uint64_t hasq[4];
    uint64_t vasq[4];

    wordchk_t wordchk;
    void*     udata;
};

struct EngineMove
{
    const char* tiles;
    const int*  squares;
    int ntiles;
    int direction;
};
// clang-format on

extern void engine_init(Engine* e, wordchk_t wordchk, void* data);

// tiles from [a-zA-Z] where lowercase = blank tile
// precondition: `squares` is sorted
extern void engine_make_move(Engine* e, const EngineMove* move);


// TEMP TEMP -- utility functions to delete

// 0 = meets all xchecks, >0 is the first square that violated
extern int engine_xchk(const Engine* e, const EngineMove* m); // TEMP TEMP

extern void engine_print_anchors(const Engine* e);

extern const char* SQ_(int sq) noexcept;
