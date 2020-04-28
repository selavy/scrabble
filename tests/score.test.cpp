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

    SECTION("Score 'H8 UNIFY 30', '11E sulfured 98', '12A bazoo 40', '13A yea 43'")
    {
        // auto rack1 = make_rack("UNRYFIA");
        auto move1 = scrabble::Move::from_isc_spec("H8 unify 30");
        auto engine_move1 = scrabble::EngineMove::make(&engine, move1);
        int  score1 = cicero_score_move_fast(&engine, &engine_move1.move);
        CHECK(score1 == 30);
        cicero_make_move(&engine, &engine_move1.move);

        // auto rack2 = make_rack("RDSELUU");
        auto move2 = scrabble::Move::from_isc_spec("11E sulfured 98");
        auto engine_move2 = scrabble::EngineMove::make(&engine, move2);
        int  score2 = cicero_score_move_fast(&engine, &engine_move2.move);
        CHECK(score2 == 98);
        cicero_make_move(&engine, &engine_move2.move);

        // auto rack3 = make_rack("OBATORZ");
        auto move3 = scrabble::Move::from_isc_spec("12A bazoo 40");
        auto engine_move3 = scrabble::EngineMove::make(&engine, move3);
        int  score3 = cicero_score_move_fast(&engine, &engine_move3.move);
        CHECK(score3 == 40);
        cicero_make_move(&engine, &engine_move3.move);

        auto move4 = scrabble::Move::from_isc_spec("13A yea 43");
        auto engine_move4 = scrabble::EngineMove::make(&engine, move4);
        int  score4 = cicero_score_move_fast(&engine, &engine_move4.move);
        CHECK(score4 == 43);
        int  old_score4 = cicero_make_move(&engine, &engine_move4.move);
        CHECK(old_score4 == 43);

    }
}

TEST_CASE("Move played between 2 adjacent tiles")
{
    auto cb = make_callbacks({});
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    struct TestCase { std::string rack; std::string isc; };
    const std::vector<TestCase> moves = {
        { "EIFTWRU", "8D fruit     24" },
        { "LAAOH?O", "D4 aloof     16" },
        { "BZRWADE", "C1 brazed    52" },
        { "GE?GWPI", "8A eggfruit  39" },
        { "?RBKLHA", "5A Shellbark 84" },
        { "AEDAOSI", "2A aeradios  72" },
        { "?LWIOPA", "1F plow      39" },
        { "IDIAORT", "1A dab       29" },
        { "GIA?EIE", "7G gie       14" },
        // { "CVEIAL?", "6I ave       21" },
    };
    for (auto&& [rack, isc] : moves) {
        auto move        = scrabble::Move::from_isc_spec(isc);
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto cicero_move = engine_move.move;
        auto score       = cicero_score_move_fast(&engine, &cicero_move);
        CHECK(score == move.score);
        cicero_make_move(&engine, &cicero_move);
    }

    {
        TestCase test_case{ "CVEIAL?", "6I ave       21" };
        auto&& rack      = test_case.rack;
        auto&& isc       = test_case.isc;
        auto move        = scrabble::Move::from_isc_spec(isc);
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto cicero_move = engine_move.move;
        auto score       = cicero_score_move_fast(&engine, &cicero_move);
        CHECK(score == move.score);
        cicero_make_move(&engine, &cicero_move);
    }
}

TEST_CASE("Test from CF589 vs whatnoloan")
{
    auto cb = make_callbacks({});
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    struct TestCase { std::string rack; std::string isc; };
    const std::vector<TestCase> moves = {
        { "EIFTWRU", "8D fruit     24" },
        { "LAAOH?O", "D4 aloof     16" },
        { "BZRWADE", "C1 brazed    52" },
        { "GE?GWPI", "8A eggfruit  39" },
        { "?RBKLHA", "5A Shellbark 84" },
        { "AEDAOSI", "2A aeradios  72" },
        { "?LWIOPA", "1F plow      39" },
        { "IDIAORT", "1A dab       29" },
        { "GIA?EIE", "7G gie       14" },
        { "CVEIAL?", "6I ave       21" },
        { "TXROIRR", "K3 oxter     24" },
        { "HTJCIL?", "L2 chi       39" },
        { "CRIRUSA", "8K scaur     43" },
        { "YNNJTL?", "M7 janny     24" },
        { "SAIREII", "10L inia      6" },
        { "LE?DITN", "O4 interlaRd 62" },
        { "TITEORS", "3F to        12" },
        // { "MOYNIMN", "11I minny    22" },
    };
    for (auto&& [rack, isc] : moves) {
        auto move        = scrabble::Move::from_isc_spec(isc);
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto cicero_move = engine_move.move;
        auto score       = cicero_score_move_fast(&engine, &cicero_move);
        CHECK(score == move.score);
        cicero_make_move(&engine, &cicero_move);
    }

    ////////////////////////////////////////////////////////////////////////
    // WORKING ON THIS TEST CASE. For some reason the cross-score for L11
    // if 4 instead of 1
    ////////////////////////////////////////////////////////////////////////

    {
        TestCase test_case{ "MOYNIMN", "11I minny    22" };
        auto&& rack      = test_case.rack;
        auto&& isc       = test_case.isc;
        auto move        = scrabble::Move::from_isc_spec(isc);
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto cicero_move = engine_move.move;
        auto score       = cicero_score_move_fast(&engine, &cicero_move);
        CHECK(score == move.score);
        cicero_make_move(&engine, &cicero_move);
    }
}
