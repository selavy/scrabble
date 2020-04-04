#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <fstream>



#if 0

#define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__);

constexpr int NumRows = 15;
constexpr int NumCols = 15;
constexpr int NumSquares = 255;
constexpr int NumRackTiles = 7;
constexpr int NumTileKinds = 27;
constexpr int NumTotalTiles = 100;
constexpr int NumPlayers = 2;
constexpr int MaxWordLen = NumRackTiles;

using Tile = int;
using TileFreq = int;

enum {
    Tile_A = 0,
    Tile_B = 1,
    Tile_C = 2,
    Tile_D = 3,
    Tile_E = 4,
    Tile_F = 5,
    Tile_G = 6,
    Tile_H = 7,
    Tile_I = 8,
    Tile_J = 9,
    Tile_K = 10,
    Tile_L = 11,
    Tile_M = 12,
    Tile_N = 13,
    Tile_O = 14,
    Tile_P = 15,
    Tile_Q = 16,
    Tile_R = 17,
    Tile_S = 18,
    Tile_T = 19,
    Tile_U = 20,
    Tile_V = 21,
    Tile_W = 22,
    Tile_X = 23,
    Tile_Y = 24,
    Tile_Z = 25,
    Tile_Blank = 26,
    Tile_Empty = 27,
};

enum Direction
{
    Direction_Horz = 1,
    Direction_Vert = 10,
};

// #define ASIZE(x) (sizeof(x)/sizeof(x[0]));
#define ASIZE(x) std::extent<decltype(x)>::value

struct GameState
{
    Tile board[NumSquares];
    TileFreq bag[NumTileKinds];
    Tile draw_order[NumTotalTiles];
    int num_bag_tiles;
    int scores[NumPlayers];
    TileFreq racks[2][NumRackTiles];
    int move;
};

using Move = Tile[MaxWordLen];

constexpr TileFreq starting_frequencies[27] = {
     9,  2,  2,  4, 12,  2,  3,  2,
     9,  1,  1,  4,  2,  6,  8,  2,
     1,  6,  4,  6,  4,  2,  2,  1,
     2, 1, 2,
};

GameState make_game() noexcept
{
    GameState gs;
    std::fill(std::begin(gs.board), std::end(gs.board), static_cast<Tile>(Tile_Empty));
    for (int i = 0; i < NumTileKinds; ++i) {
        int cnt = starting_frequencies[i];
        gs.bag[i] = cnt;
        gs.num_bag_tiles = 0;
        while  (cnt-- > 0) {
            gs.draw_order[gs.num_bag_tiles++] = i;
        }
    }
    assert(gs.num_bag_tiles == NumTotalTiles);
    srand(42);
    std::random_shuffle(std::begin(gs.draw_order), std::end(gs.draw_order));
    std::random_shuffle(std::begin(gs.draw_order), std::end(gs.draw_order));
    std::fill(std::begin(gs.scores), std::end(gs.scores), 0);
    std::fill(std::begin(gs.racks[0]), std::end(gs.racks[0]), Tile_Empty);
    std::fill(std::begin(gs.racks[1]), std::end(gs.racks[1]), Tile_Empty);
    gs.move = 0;
    return gs;
}

#if 0
bool make_move(GameState* s, int player, int sq, Direction dir, Move move)
{
    if (player != (s->move % 2)) {
        DEBUG("make_move: incorrect player=%d for move=%d", player, s->move);
    }
    Tile* board = &s->board[0];
    int step  = dir == Direction_Horz ? 1 : NumCols;
    for (int i = 0; i < NumRackTiles && move[i] != Tile_Empty; ++i) {
        assert(Tile_A <= move[i] && move[i] <= Tile_Blank);

    }
}
#endif

int main(int argc, const char** argv)
{
    GameState gs = make_game();

    return 0;
}

#endif
