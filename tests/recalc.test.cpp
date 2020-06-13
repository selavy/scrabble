#include <catch2/catch.hpp>
#include "test_helpers.h"

bool isanchorsq(const cicero& e, int sq)
{
    const auto* asq = e.asqs;
    const int m = sq / 64;
    const int n = sq % 64;
    return (asq[m] & (uint64_t)(1ull << n)) != 0;
}

TEST_CASE("Rebuild state after H8 APPLE")
{
    auto cb = make_callbacks({});
    cicero engine1;
    cicero engine2;
    cicero_savepos sp;
    cicero_init(&engine1, cb.make_callbacks());

    char board[225];
    for (int sq = 0; sq < 225; ++sq) {
        board[sq] = cicero_tile_on_square(&engine1, sq);
    }
    cicero_init_from_position(&engine2, board);

    for (int sq = 0; sq < 225; ++sq) {
        CHECK(engine1.vals[sq] == engine2.vals[sq]);
        REQUIRE(engine1.hscr[sq] == engine2.hscr[sq]);
        // CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
        // CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
        // CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
        // CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
    }
}
