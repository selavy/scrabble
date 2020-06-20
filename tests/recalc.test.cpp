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

void apply_move(cicero* engine, std::string isc_move)
{
    auto smove = scrabble::Move::from_isc_spec(isc_move.c_str());
    auto emove = scrabble::EngineMove::make(engine, smove);
    auto* move = &emove.move;
    cicero_savepos sp;
    cicero_make_move(engine, &sp, move);
}

TEST_CASE("Rebuild state after H8 APPLE")
{
    auto cb = make_callbacks({});
    cicero engine1;
    cicero engine2;
    cicero_savepos sp;
    char board[225];
    cicero_init(&engine1, cb.make_callbacks());
    cicero_init(&engine2, cb.make_callbacks());

    auto load_position = [&cb, &board](const auto* e1, auto* e2)
    {
        cicero_init(e2, cb.make_callbacks());
        copy_position(*e1, board);
        cicero_load_position(e2, board);
    };

    {
        copy_position(engine1, board);
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    {
        apply_move(&engine1, "H8 flip");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }

    }

    {
        apply_move(&engine1, "8H ferrets");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    {
        apply_move(&engine1, "L7 lengths");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    {
        apply_move(&engine1, "7I we");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    {
        apply_move(&engine1, "13I zoOs");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    {
        apply_move(&engine1, "12K ThOSe");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }

    {
        apply_move(&engine1, "M12 Swim");
        load_position(&engine1, &engine2);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }
}
