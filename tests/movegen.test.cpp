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
    cicero_savepos sp;
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
        cicero_make_move(&engine, &sp, &move);

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
    cicero_savepos sp;
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    SECTION("Score 'H8 UNIFY 30' first move")
    {
        auto rack = make_rack("UNRYFIA");
        auto move = scrabble::Move::from_isc_spec("H8 unify");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        cicero engine_copy;
        memcpy(&engine_copy, &engine, sizeof(engine_copy));
        auto score = cicero_make_move(&engine, &sp, &engine_move.move);
        REQUIRE(score == 30);

        cicero_undo_move(&engine, &sp, &engine_move.move);
        CHECK(memcmp(engine.vals, engine_copy.vals, sizeof(engine.vals)) == 0);
        CHECK(memcmp(engine.hscr, engine_copy.hscr, sizeof(engine.hscr)) == 0);
        CHECK(memcmp(engine.vscr, engine_copy.vscr, sizeof(engine.vscr)) == 0);
        CHECK(memcmp(engine.hchk, engine_copy.hchk, sizeof(engine.hchk)) == 0);
        CHECK(memcmp(engine.vchk, engine_copy.vchk, sizeof(engine.vchk)) == 0);
        CHECK(memcmp(engine.asqs, engine_copy.asqs, sizeof(engine.asqs)) == 0);

        auto score2 = cicero_make_move(&engine, &sp, &engine_move.move);
        CHECK(score2 == score);
    }

    SECTION("Score 'H4 hazer 42' first move")
    {
        auto rack = make_rack("AREHAUZ");
        auto move = scrabble::Move::from_isc_spec("H4 hazer 42");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &sp, &engine_move.move);
        REQUIRE(score == 42);

        cicero_undo_move(&engine, &sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &sp, &engine_move.move);
        CHECK(score2 == score);
    }

    SECTION("Score '8D where 30' first move")
    {
        auto rack = make_rack("HRWERIE");
        auto move = scrabble::Move::from_isc_spec("8D where 30");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &sp, &engine_move.move);
        REQUIRE(score == 30);

        cicero_undo_move(&engine, &sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &sp, &engine_move.move);
        CHECK(score2 == score);
    }

    SECTION("Score 'H7 quIz 42' first move")
    {
        auto rack = make_rack("UTI?GQZ");
        auto move = scrabble::Move::from_isc_spec("H7 quIz 42");
        auto engine_move = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &sp, &engine_move.move);
        REQUIRE(score == 42);

        cicero_undo_move(&engine, &sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &sp, &engine_move.move);
        CHECK(score2 == score);
    }
}

TEST_CASE("Convert cicero_move -> cicero_move2", "[cicero_move2]")
{
    auto cb = make_callbacks({});
    cicero_savepos sp;
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    auto check_word = [](const cicero_move2& m, const char* const word)
    {
        const char* p = word;
        int i = 0;
        for (; *p != '\0'; ++p, ++i) {
            CHECK(m.tiles[i] == *p);
        }
        for (; i < 16; ++i) {
            CHECK(m.tiles[i] == 0);
        }
    };

    SECTION("Horizontal Move")
    {
        auto move = scrabble::Move::from_isc_spec("H7 quIz 42");
        auto emove = scrabble::EngineMove::make(&engine, move);
        auto& move1 = emove.move;
        auto move2 = cicero_make_move2(&engine, &move1);
        CHECK(move2.square    == move.square.value());
        CHECK(move2.direction == static_cast<int>(move.direction));
        check_word(move2, "QUiZ");
        std::cout << move << " -> " << move2 << "\n";
    }

    SECTION("Vertical Move")
    {
        auto move = scrabble::Move::from_isc_spec("7H quIz 42");
        auto emove = scrabble::EngineMove::make(&engine, move);
        auto& move1 = emove.move;
        auto move2 = cicero_make_move2(&engine, &move1);
        CHECK(move2.square    == move.square.value());
        CHECK(move2.direction == static_cast<int>(move.direction));
        check_word(move2, "QUiZ");
        std::cout << move << " -> " << move2 << "\n";
    }

    SECTION("Couple moves")
    {
        // clang-format off
        const std::vector<std::string> isc_moves = {
            "H7  zag     26",
            "I6  bam     24",
            "J5  tag     25",
            "8F  tram     6",
            "K5  od      16",
            "L4  arenite 76",
            "10B pEdants 81",
            "9C  yo      20",
            "8K  jib     12",
            "N6  toeclip 78",
            "O6  ohs     67",
            "F6  qat     32",
            "12K chip    22",
            "G3  mini    20",
            "8A  oven    34",
            "4C  wellie  20",
            "O12 sire    28",
            "14J gaffer  34",
            "15F reQuite 82",
            "14A kindler 84",
            "A12 woke    45",
            "11E xu      38",
            "3C  oy      25",
            "2C  vain    24",
            "1A  oda     22",
            "5B  us      13",
        };
        // clang-format on

        for (const auto& isc_move : isc_moves) {
            auto move = scrabble::Move::from_isc_spec(isc_move);
            auto emove = scrabble::EngineMove::make(&engine, move);
            auto move2 = cicero_make_move2(&engine, &emove.move);
            CHECK(move2.square    == move.square.value());
            CHECK(move2.direction == static_cast<int>(move.direction));
            check_word(move2, move.word.c_str());
            std::cout << move << " -> " << move2 << "\n";
        }
    }

    SECTION("cruella v cf539 02/08/2016")
    {
        // clang-format off
        const std::vector<std::string> isc_moves = {
            "H8  unify    30",
            "11E sulfured 98",
            "12A bazoo    40",
            "13A yea      43",
            "12K rant     16",
            "9C  visioned 68",
            "N10 futon    32",
            "E2  Inveighs 78",
            "14H trippant 64",
            "15D graVies  87",
            "O8  cham     42",
            "7C  wig      11",
            "6B  jo       30",
            "5D  semiarid 72",
            "4H  latened  78",
            "A12 byte     27",
            "3K  low      28",
            "2M  oe       10",
            "6H  rex      58",
            "2C  taIlbone 63",
        };
        // clang-format on

        for (const auto& isc_move : isc_moves) {
            auto move = scrabble::Move::from_isc_spec(isc_move);
            auto emove = scrabble::EngineMove::make(&engine, move);
            auto move2 = cicero_make_move2(&engine, &emove.move);
            CHECK(move2.square    == move.square.value());
            CHECK(move2.direction == static_cast<int>(move.direction));
            check_word(move2, move.word.c_str());
            std::cout << move << " -> " << move2 << "\n";
        }
    }
}
