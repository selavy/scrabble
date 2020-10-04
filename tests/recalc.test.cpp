#include <catch2/catch.hpp>
#include <fmt/ostream.h>
#include "test_helpers.h"
#include "square.h"

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

TEST_CASE("Example game breaking")
{
    auto cb = make_callbacks();
    cicero engine1;
    cicero engine2;
    cicero_savepos sp;
    char board[225];
    cicero_init(&engine1, cb.make_callbacks());
    cicero_init(&engine2, cb.make_callbacks());

    const std::vector<std::string> moves = {
        "H7 oaf",
    };

    for (const auto& isc_move : moves) {
        auto smove = scrabble::Move::from_isc_spec(isc_move.c_str());
        auto emove = scrabble::EngineMove::make(&engine1, smove);
        auto* move = &emove.move;
        cicero_make_move(&engine1, &sp, move);
        // std::cout << engine1 << "\n\n";
        copy_position(engine1, board);
        cicero_init(&engine2, cb.make_callbacks());
        cicero_load_position(&engine2, board);
        for (int sq = 0; sq < 225; ++sq) {
            INFO("Checking " << hume::Square::make(sq)->name());
            INFO("Checking engine1.hchk=" << XChk{engine1.hchk[sq]});
            INFO("Checking engine2.hchk=" << XChk{engine2.hchk[sq]});
            INFO("Checking engine1.vchk=" << XChk{engine1.vchk[sq]});
            INFO("Checking engine2.vchk=" << XChk{engine2.vchk[sq]});
            CHECK(engine1.vals[sq] == engine2.vals[sq]);
            CHECK(engine1.hscr[sq] == engine2.hscr[sq]);
            CHECK(engine1.vscr[sq] == engine2.vscr[sq]);
            CHECK(engine1.hchk[sq] == engine2.hchk[sq]);
            CHECK(engine1.vchk[sq] == engine2.vchk[sq]);
            CHECK(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
        }
    }
}

TEST_CASE("Rebuild state after H8 APPLE")
{
    auto cb = make_callbacks();
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
