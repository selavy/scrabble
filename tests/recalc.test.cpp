#include <catch2/catch.hpp>
#include "test_helpers.h"
#include "square.h"

bool isanchorsq(const cicero& e, int sq)
{
    const auto* asq = e.asqs;
    const int m = sq / 64;
    const int n = sq % 64;
    return (asq[m] & (uint64_t)(1ull << n)) != 0;
}

void copy_position(const cicero& engine, char* board)
{
    for (int sq = 0; sq < 225; ++sq) {
        board[sq] = cicero_tile_on_square(&engine, sq);
    }
}

TEST_CASE("Rebuild state after H8 APPLE")
{
    auto cb = make_callbacks({});
    cicero engine1;
    cicero engine2;
    cicero_savepos sp;
    cicero_init(&engine1, cb.make_callbacks());

    char board[225];

    SECTION("Starting board")
    {
        copy_position(engine1, board);
        cicero_init(&engine2, cb.make_callbacks());
        cicero_load_position(&engine2, board);

        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            // using REQUIRE because if any fails, probably they all will
            REQUIRE(engine1.hscr[sq] == engine2.hscr[sq]);
            REQUIRE(engine1.vscr[sq] == engine2.vscr[sq]);
            // CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            // CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    SECTION("After 'H8 flip 18'")
    {
        auto smove = scrabble::Move::from_isc_spec("H8 flip 18");
        auto emove = scrabble::EngineMove::make(&engine1, smove);
        auto* move = &emove.move;
        cicero_savepos sp;
        cicero_make_move(&engine1, &sp, move);
        copy_position(engine1, board);
        cicero_init(&engine2, cb.make_callbacks());
        cicero_load_position(&engine2, board);

        // TODO: actually only care about empty squares matching src and chk
        // values
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            // using REQUIRE because if any fails, probably they all will
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            // CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }

    }
}
