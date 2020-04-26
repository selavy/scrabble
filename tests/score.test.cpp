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
