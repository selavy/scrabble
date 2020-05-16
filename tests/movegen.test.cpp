#include <catch2/catch.hpp>
#include "test_helpers.h"


TEST_CASE("Cicero first moves")
{
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
    cicero_init(&engine, cb.make_callbacks());

    SECTION("First move generate moves")
    {
        auto rack = make_rack("UNRYFIA");
        const std::vector<std::string> expect_isc = {
            // horizontal fry
            { "H6 fry" },
            { "H7 fry" },
            { "H8 fry" },
            // vertical fry
            { "8F fry" },
            { "8G fry" },
            { "8H fry" },

            // horizontal unify
            { "H4 unify" },
            { "H5 unify" },
            { "H6 unify" },
            { "H7 unify" },
            { "H8 unify" },
            // vertical unify
            { "8D unify" },
            { "8E unify" },
            { "8F unify" },
            { "8G unify" },
            { "8H unify" },
        };
        const auto expect = make_move_list(expect_isc);
        cicero_generate_legal_moves(&engine, rack);
        auto legal_moves = cb.sorted_legal_moves();
        CHECK(legal_moves == expect);
    }

    SECTION("First move with blank tile")
    {
        auto rack = make_rack("APPL?ZW");
        std::vector<std::string> expect_isc = {
            // horizontal applE
            { "H4 applE" },
            { "H5 applE" },
            { "H6 applE" },
            { "H7 applE" },
            { "H8 applE" },
            // vertical applE
            { "8D applE" },
            { "8E applE" },
            { "8F applE" },
            { "8G applE" },
            { "8H applE" },
        };
        auto expect = make_move_list(expect_isc);
        cicero_generate_legal_moves(&engine, rack);
        auto legal_moves = cb.sorted_legal_moves();
        CHECK(legal_moves == expect);
    }

    SECTION("Second move after H8 unify")
    {
        char tiles[5]   = {      'U',      'N',       'I',       'F',       'Y' };
        int  squares[5] = { IX("H8"), IX("H9"), IX("H10"), IX("H11"), IX("H12") };
        cicero_move move;
        move.tiles   = &tiles[0];
        move.squares = &squares[0];
        move.ntiles  = 5;
        move.direction = CICERO_HORZ;
        cicero_make_move(&engine, &move);

        auto rack = make_rack("RDSELUU");
        auto expect = scrabble::Move::from_isc_spec("11E sulfured");
        cicero_generate_legal_moves(&engine, rack);
        auto legal_moves = cb.sorted_legal_moves();
        REQUIRE(legal_moves.size() == 1);
        CHECK(legal_moves[0] == expect);
    }
}

TEST_CASE("Parse ISC moves")
{
    SECTION("8G flip 18")
    {
        std::string spec = "8G flip 18";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("8G"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "FLIP");
        REQUIRE(move.score     == 18);
    }

    SECTION("I7 kiva 20")
    {
        std::string spec = "I7 kiva 20";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("I7"));
        REQUIRE(move.direction == scrabble::Direction::Horz);
        REQUIRE(move.word      == "KIVA");
        REQUIRE(move.score     == 20);
    }

    SECTION("J10 ae 8")
    {
        std::string spec = "J10 ae 8";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("J10"));
        REQUIRE(move.direction == scrabble::Direction::Horz);
        REQUIRE(move.word      == "AE");
        REQUIRE(move.score     == 8);
    }

    SECTION("11J exon 22")
    {
        std::string spec = "11J exon 22";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("11J"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "EXON");
        REQUIRE(move.score     == 22);
    }

    SECTION("M9 lun 4")
    {
        std::string spec = "M9 lun 4";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("M9"));
        REQUIRE(move.direction == scrabble::Direction::Horz);
        REQUIRE(move.word      == "LUN");
        REQUIRE(move.score     == 4);
    }

    SECTION("12K up 25")
    {
        std::string spec = "12K up 25";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("12K"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "UP");
        REQUIRE(move.score     == 25);
    }

    SECTION("Parse no score: 12K up")
    {
        std::string spec = "12K up";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IXSQ("12K"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "UP");
        REQUIRE(move.score     == -1);
    }
}

TEST_CASE("Undo move should return to old state")
{
    auto cb = make_callbacks({});
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    SECTION("Score 'H8 UNIFY 30' first move")
    {
        auto rack = make_rack("UNRYFIA");
        auto move = scrabble::Move::from_isc_spec("H8 unify");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        cicero engine_copy;
        memcpy(&engine_copy, &engine, sizeof(engine_copy));
        auto score = cicero_make_move(&engine, &engine_move.move);
        REQUIRE(score == 30);

        cicero_undo_move(&engine, &engine.sp, &engine_move.move);
        CHECK(memcmp(engine.vals, engine_copy.vals, sizeof(engine.vals)) == 0);
        CHECK(memcmp(engine.hscr, engine_copy.hscr, sizeof(engine.hscr)) == 0);
        CHECK(memcmp(engine.vscr, engine_copy.vscr, sizeof(engine.vscr)) == 0);
        CHECK(memcmp(engine.hchk, engine_copy.hchk, sizeof(engine.hchk)) == 0);
        CHECK(memcmp(engine.vchk, engine_copy.vchk, sizeof(engine.vchk)) == 0);
        CHECK(memcmp(engine.asqs, engine_copy.asqs, sizeof(engine.asqs)) == 0);

        auto score2 = cicero_make_move(&engine, &engine_move.move);
        CHECK(score2 == score);
    }

    SECTION("Score 'H4 hazer 42' first move")
    {
        auto rack = make_rack("AREHAUZ");
        auto move = scrabble::Move::from_isc_spec("H4 hazer 42");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &engine_move.move);
        REQUIRE(score == 42);

        cicero_undo_move(&engine, &engine.sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &engine_move.move);
        CHECK(score2 == score);
    }

    SECTION("Score '8D where 30' first move")
    {
        auto rack = make_rack("HRWERIE");
        auto move = scrabble::Move::from_isc_spec("8D where 30");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &engine_move.move);
        REQUIRE(score == 30);

        cicero_undo_move(&engine, &engine.sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &engine_move.move);
        CHECK(score2 == score);
    }

    SECTION("Score 'H7 quIz 42' first move")
    {
        auto rack = make_rack("UTI?GQZ");
        auto move = scrabble::Move::from_isc_spec("H7 quIz 42");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &engine_move.move);
        REQUIRE(score == 42);

        cicero_undo_move(&engine, &engine.sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &engine_move.move);
        CHECK(score2 == score);
    }
}
