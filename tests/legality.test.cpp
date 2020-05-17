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

    SECTION("Check legality of 'H3 unify' 2")
    {
        const char tiles[]   = { 'U',      'N',      'I',      'F',     'Y'       };
        const int  squares[] = { IX("H3"), IX("H4"), IX("H5"), IX("H6"), IX("H7") };
        const int  ntiles = static_cast<int>(std::extent<decltype(tiles)>::value);
        cicero_move move;
        move.tiles   = tiles;
        move.squares = squares;
        move.ntiles  = ntiles;
        move.direction = CICERO_HORZ;
        auto result = cicero_legal_move(&engine, &move);
        CHECK(result != CICERO_LEGAL_MOVE);
    }

    struct TestCase
    {
        std::vector<char> tiles;
        std::vector<int > squares;
        cicero_direction  direction;
        bool              expect;
    };
    std::vector<TestCase> testcases = {
        {
            { 'U',      'N',      'I',      'F',     'Y'       },
            { IX("H3"), IX("H4"), IX("H5"), IX("H6"), IX("H7") },
            CICERO_HORZ,
            false,
        },
        {
            { 'U',      'N',      'I',       'F',      'Y'        },
            { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") },
            CICERO_HORZ,
            true,
        },
        {
            { 'U',      'N',      'I',       'F',      'Z'        },
            { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") },
            CICERO_HORZ,
            false,
        },
        {
            { 'U',      'N',      'I',       'B',      'Y'        },
            { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") },
            CICERO_HORZ,
            false,
        },
        {
            { 'A',      'N',      'I',       'F',      'Y'        },
            { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") },
            CICERO_HORZ,
            false,
        },
        {
            { 'U',      'C',      'I',       'F',      'Y'        },
            { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") },
            CICERO_HORZ,
            false,
        },
    };

    for (auto& t: testcases) {
        cicero_move move;
        REQUIRE(t.tiles.size() == t.squares.size());
        move.tiles = t.tiles.data();
        move.squares = t.squares.data();
        move.ntiles = static_cast<int>(t.tiles.size());
        move.direction = t.direction;
        auto result = cicero_legal_move(&engine, &move);
        CHECK((result == CICERO_LEGAL_MOVE) == t.expect);
    }
}
