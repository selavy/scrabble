#include <catch2/catch.hpp>
#include "test_helpers.h"
#include <vector>
#include <utility>


TEST_CASE("Check first move legality")
{
    // std::vector<std::pair<std::string, bool>>> test_ca
    const std::vector<std::string> words = {
        "UNIFY",
        "FRY",
        "APPLE",
        "ORANGE",
        "HELLO",
        "WORLD",
        "SULFURED",
    };

    auto cb = make_callbacks(words);
    cicero engine;
    cicero_savepos sp;
    cicero_init(&engine, cb.make_callbacks());

    SECTION("Check legality of 'H8 unify'")
    {
        auto move = scrabble::Move::from_isc_spec("H8 unify");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto result = cicero_legal_move(&engine, &engine_move.move);
        CHECK(result == CICERO_LEGAL_MOVE);
    }

    SECTION("Check legality of 'H8 unify' 2")
    {
        const char tiles[]   = { 'U',      'N',      'I',       'F',      'Y'        };
        const int  squares[] = { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") };
        const int  ntiles = static_cast<int>(std::extent<decltype(tiles)>::value);
        cicero_move move;
        move.tiles   = tiles;
        move.squares = squares;
        move.ntiles  = ntiles;
        move.direction = CICERO_HORZ;
        auto result = cicero_legal_move(&engine, &move);
        CHECK(result == CICERO_LEGAL_MOVE);
    }

}
