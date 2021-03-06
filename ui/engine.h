#pragma once

#include <cstdint>
#include <array>

// ----------------------------------------------------------------------------
// EngineTrie
// ----------------------------------------------------------------------------

// TODO: put this somewhere else?
#include <map>
#include <vector>
#include <string>
#include <iosfwd>
#include <cassert>

struct EngineTrie
{
    struct Node {
        char                value    = ' '; // TEMP TEMP
        bool                terminal = false;
        std::map<char, int> children;

    };

    EngineTrie() {
        nodes_.emplace_back();
    }
    void insert(const char* word);
    void insert(const std::string& w) { insert(w.c_str()); }
    bool is_word(const char* word) const;
    bool is_word(const std::string& w) const { return is_word(w.c_str()); }
    std::vector<char> children(const char* prefix) const
    {
        [[maybe_unused]] bool unused;
        return children(prefix, unused);
    }
    std::vector<char> children(const char* prefix, bool& is_terminal) const;
    void print(std::ostream& os) const;
    void print_(std::ostream& os, int curr, std::string& word) const;

    static constexpr char safech(char c) noexcept {
        if ('A' <= c && c <= 'Z') {
            return c;
        } else if ('a' <= c && c <= 'z') {
            return 'A' + (c - 'a');
        } else {
            assert(0 && "invalid character");
            return '?';
        }
    }

    std::vector<Node> nodes_;
};

std::ostream& operator<<(std::ostream& os, const EngineTrie& t);

// ----------------------------------------------------------------------------


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

// move back into cpp
inline /*constexpr*/ int SQIX(char row, int col) noexcept {
    int r = row - 'A';
    int c = col - 1;
    // return r*DIM + c;
    return r*15 + c;
}

//-----------------------------------------------------------------------------
// TEMP TEMP -- utility functions to delete
//-----------------------------------------------------------------------------

// 0 = meets all xchecks, >0 is the first square that violated
extern int engine_xchk(const Engine* e, const EngineMove* m); // TEMP TEMP

extern void engine_print_anchors(const Engine* e);

extern const char* SQ_(int sq) noexcept;

#include <memory>

// TEMP TEMP
extern std::unique_ptr<char[]> mask_buffer(uint32_t m);
#define MBUF(m) mask_buffer(m).get()
#define MM(x, m) ((((x) & (m)) != 0) ? 1 : 0)
// TEMP TEMP
