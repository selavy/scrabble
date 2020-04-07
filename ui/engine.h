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

void engine_init(Engine* e, wordchk_t wordchk, void* data);

// tiles from [a-zA-Z] where lowercase = blank tile
// precondition: `squares` is sorted
void engine_make_move(Engine* e, const EngineMove* move);

// 0 = meets all xchecks, >0 is the first square that violated
int engine_xchk(const Engine* e, const EngineMove* m); // TEMP TEMP

const char* SQ_(int sq) noexcept; // TEMP TEMP
