#pragma once

#include <cstdint>
#include <array>


typedef int (*WordCheck)(void* data, const char* word);
typedef void (*LegalMove)(void* data, const char* word, int sq, int dir);

// clang-format off
struct Engine
{
    char     vals[225]; // TODO(peter): maybe remove and pass in from UI?
    uint32_t hchk[225];
    uint32_t vchk[225];
    uint64_t hasq[4];
    uint64_t vasq[4];

    WordCheck wordchk;
    void*     wordchk_data;
    LegalMove legalmv;
    void*     legalmv_data;
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

extern void engine_init(Engine* e, WordCheck word_check, void* word_check_data, LegalMove legal_move, void* legal_move_data);

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
