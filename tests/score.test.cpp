#include <catch2/catch.hpp>
#include "test_helpers.h"


TEST_CASE("Cicero score first move")
{
    const std::vector<std::string> words = {
        "UNIFY",
        "HAZER",
        "WHERE",
        "QUIZ",
    };

    auto cb = make_callbacks(words);
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    SECTION("Score 'H8 UNIFY 30' first move")
    {
        auto rack = make_rack("UNRYFIA");
        auto move = scrabble::Move::from_isc_spec("H8 unify");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        int  score = cicero_score_move_fast(&engine, &engine_move.move);
        REQUIRE(score == 30);
    }

    SECTION("Score 'H4 hazer 42' first move")
    {
        auto rack = make_rack("AREHAUZ");
        auto move = scrabble::Move::from_isc_spec("H4 hazer 42");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        int  score = cicero_score_move_fast(&engine, &engine_move.move);
        REQUIRE(score == 42);
    }

    SECTION("Score '8D where 30' first move")
    {
        auto rack = make_rack("HRWERIE");
        auto move = scrabble::Move::from_isc_spec("8D where 30");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        int  score = cicero_score_move_fast(&engine, &engine_move.move);
        REQUIRE(score == 30);
    }

    SECTION("Score 'H7 quIz 42' first move")
    {
        auto rack = make_rack("UTI?GQZ");
        auto move = scrabble::Move::from_isc_spec("H7 quIz 42");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        int  score = cicero_score_move_fast(&engine, &engine_move.move);
        REQUIRE(score == 42);
    }
}

TEST_CASE("Cicero score second move")
{
    const std::vector<std::string> words = {
        "UNIFY",
        "HAZER",
        "WHERE",
        "QUIZ",
        "SULFURED"
    };

    auto cb = make_callbacks(words);
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    SECTION("Score 'H8 UNIFY 30', '11E sulfured 98', '12A bazoo 40'")
    {
        // auto rack1 = make_rack("UNRYFIA");
        auto move1 = scrabble::Move::from_isc_spec("H8 unify 30");
        auto engine_move1 = scrabble::EngineMove::make(&engine, move1);
        int  score1 = cicero_score_move_fast(&engine, &engine_move1.move);
        REQUIRE(score1 == 30);
        cicero_make_move(&engine, &engine_move1.move);

        // auto rack2 = make_rack("RDSELUU");
        auto move2 = scrabble::Move::from_isc_spec("11E sulfured 98");
        auto engine_move2 = scrabble::EngineMove::make(&engine, move2);
        int  score2 = cicero_score_move_fast(&engine, &engine_move2.move);
        REQUIRE(score2 == 98);

        // auto rack3 = make_rack("OBATORZ");
        auto move3 = scrabble::Move::from_isc_spec("12A bazoo 40");
        auto engine_move3 = scrabble::EngineMove::make(&engine, move3);
        int  score3 = cicero_score_move_fast(&engine, &engine_move3.move);
        REQUIRE(score3 == 40);
    }
}
