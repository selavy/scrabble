#include <catch2/catch.hpp>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cicero/cicero.h>
#include <scrabble.h>
#include <mafsa++.h>
#include "test_data.h"


struct Callbacks
{
    using Move = scrabble::Move;

    Callbacks(Mafsa&& m) noexcept : mafsa_{std::move(m)}, legal_moves_{} {}

    static cicero_edges prefix_edges(void* data, const char* prefix) noexcept
    {
        auto* self = reinterpret_cast<const Callbacks*>(data);
        return self->prefix_edges_(prefix);
    }

    static void on_legal_move(void* data, const char* word, int square, int direction) noexcept
    {
        auto* self = reinterpret_cast<Callbacks*>(data);
        return self->on_legal_move_(word, square, direction);
    }

    void clear_legal_moves() noexcept
    {
        legal_moves_.clear();
    }

    std::vector<Move> sorted_legal_moves() const noexcept
    {
        auto result = legal_moves_;
        std::sort(result.begin(), result.end());
        return result;
    }

    cicero_callbacks make_callbacks() const noexcept
    {
        cicero_callbacks cb;
        cb.onlegal = &Callbacks::on_legal_move;
        cb.getedges = &Callbacks::prefix_edges;
        cb.onlegaldata = this;
        cb.getedgesdata = this;
        return cb;
    }

private:
    cicero_edges prefix_edges_(const char* prefix) const noexcept
    {
        cicero_edges out;
        auto edges = mafsa_.get_edges(prefix);
        static_assert(sizeof(edges) == sizeof(out));
        memcpy(&out, &edges, sizeof(out));
        return out;
    }

    void on_legal_move_(const char* word, int square, int direction)
    {
        legal_moves_.emplace_back();
        legal_moves_.back().square = square;
        legal_moves_.back().direction = static_cast<scrabble::Direction>(direction);
        legal_moves_.back().word = word;
        legal_moves_.back().score = -1;
    }

    Mafsa             mafsa_;
    std::vector<Move> legal_moves_;
};

Callbacks make_callbacks(const std::vector<std::string>& words = DICT)
{
    MafsaBuilder builder;
    for (const auto& word : words) {
        REQUIRE(builder.insert(word) == true);
    }
    auto maybe_mafsa = builder.finish();
    REQUIRE(static_cast<bool>(maybe_mafsa) == true);
    return Callbacks(std::move(*maybe_mafsa));
}

cicero_rack make_rack(std::string tiles)
{
    cicero_rack rack;
    memset(&rack, 0, sizeof(rack));
    for (char tile : tiles) {
        cicero_rack_add_tile(&rack, tile);
    }
    return rack;
}

std::vector<scrabble::Move> make_move_list(const std::vector<std::string>& isc_specs)
{
    std::vector<scrabble::Move> result;
    for (const auto& isc_spec : isc_specs) {
        result.emplace_back(scrabble::Move::from_isc_spec(isc_spec));
    }
    std::sort(result.begin(), result.end());
    return result;
}

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
    cicero_movegen movegen;
    cicero_movegen_init(&movegen, cb.make_callbacks());

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
        cicero_movegen_generate(&movegen, rack);
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
        cicero_movegen_generate(&movegen, rack);
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
        cicero_movegen_make_move(&movegen, &move);

        auto rack = make_rack("RDSELUU");
        auto expect = scrabble::Move::from_isc_spec("11E sulfured");
        cicero_movegen_generate(&movegen, rack);
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
        REQUIRE(move.square    == IX("8G"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "FLIP");
        REQUIRE(move.score     == 18);
    }

    SECTION("I7 kiva 20")
    {
        std::string spec = "I7 kiva 20";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IX("I7"));
        REQUIRE(move.direction == scrabble::Direction::Horz);
        REQUIRE(move.word      == "KIVA");
        REQUIRE(move.score     == 20);
    }

    SECTION("J10 ae 8")
    {
        std::string spec = "J10 ae 8";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IX("J10"));
        REQUIRE(move.direction == scrabble::Direction::Horz);
        REQUIRE(move.word      == "AE");
        REQUIRE(move.score     == 8);
    }

    SECTION("11J exon 22")
    {
        std::string spec = "11J exon 22";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IX("11J"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "EXON");
        REQUIRE(move.score     == 22);
    }

    SECTION("M9 lun 4")
    {
        std::string spec = "M9 lun 4";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IX("M9"));
        REQUIRE(move.direction == scrabble::Direction::Horz);
        REQUIRE(move.word      == "LUN");
        REQUIRE(move.score     == 4);
    }

    SECTION("12K up 25")
    {
        std::string spec = "12K up 25";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IX("12K"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "UP");
        REQUIRE(move.score     == 25);
    }

    SECTION("Parse no score: 12K up")
    {
        std::string spec = "12K up";
        auto move = scrabble::Move::from_isc_spec(spec);
        REQUIRE(move.square    == IX("12K"));
        REQUIRE(move.direction == scrabble::Direction::Vert);
        REQUIRE(move.word      == "UP");
        REQUIRE(move.score     == -1);
    }
}
