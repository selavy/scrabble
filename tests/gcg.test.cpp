#include <catch2/catch.hpp>
#include "gcg.h"

using namespace hume;

TEST_CASE("Parse pragmata", "[gcg]")
{
    SECTION("Player")
    {
        std::string line = "#player1 Jamie James Chew";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(static_cast<bool>(result) == true);
        auto* pragma = std::get_if<gcg::Pragmata>(&*result);
        REQUIRE(pragma != nullptr);
        CHECK(pragma->keyword == "player1");
        CHECK(pragma->arguments.at(0) == "Jamie");
        CHECK(pragma->arguments.at(1) == "James");
        CHECK(pragma->arguments.at(2) == "Chew");
        CHECK(pragma->arguments.size() == 3);
    }

    SECTION("Title")
    {
        std::string line = "#title Sample Game";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(static_cast<bool>(result) == true);
        auto* pragma = std::get_if<gcg::Pragmata>(&*result);
        REQUIRE(pragma != nullptr);
        CHECK(pragma->keyword == "title");
        CHECK(pragma->arguments.at(0) == "Sample");
        CHECK(pragma->arguments.at(1) == "Game");
        CHECK(pragma->arguments.size() == 2);
    }

    // currently don't fully support this type of pragma because it has
    // embedded whitespace
#if 0
    SECTION("Comment")
    {
        std::string line = R"(#comment date="2006-06-02 13:05"; author="John Chew <jjchew@math.utoronto.ca">; text="\"Hello, world!\""")";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(static_cast<bool>(result) == true);
        auto* pragma = std::get_if<gcg::Pragmata>(&*result);
        REQUIRE(pragma != nullptr);
        CHECK(pragma->keyword == "comment");
        CHECK(pragma->arguments.at(0) == R"(date="2006-06-02 13:05";)");
        CHECK(pragma->arguments.at(1) == R"(author="John Chew <jjchew@math.utoronto.ca">;)");
        CHECK(pragma->arguments.at(2) == R"(text="\"Hello, world!\"")");
        CHECK(pragma->arguments.size() == 3);
    }
#endif

    SECTION("Rack")
    {
        std::string line = "#rack2 RETAIN?	";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(static_cast<bool>(result) == true);
        auto* pragma = std::get_if<gcg::Pragmata>(&*result);
        REQUIRE(pragma != nullptr);
        CHECK(pragma->keyword == "rack2");
        CHECK(pragma->arguments.at(0) == "RETAIN?");
        CHECK(pragma->arguments.size() == 1);
    }
}

TEST_CASE("Play", "[gcg]")
{
    SECTION(">David: ANTHER? n8 ANoTHER +73 416	")
    {
        std::string line = ">David: ANTHER? n8 ANoTHER +73 416	";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(static_cast<bool>(result) == true);
        auto* move = std::get_if<gcg::Play>(&*result);
        REQUIRE(move != nullptr);
        CHECK(move->player == "David");
        CHECK(move->rack   == "ANTHER?");
        CHECK(move->square == *Square::make(118));
        CHECK(move->word   == "anOther");
        CHECK(move->score  == 73);
        CHECK(move->total_score == 416);
    }
}

TEST_CASE("Passed Turn", "[gcg]")
{
    SECTION(">Randy: U - +0 380")
    {
        std::string line = ">Randy: U - +0 380";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* pass = std::get_if<gcg::PassedTurn>(&*result);
        REQUIRE(pass != nullptr);
        CHECK(pass->player == "Randy");
        CHECK(pass->rack == "U");
        CHECK(pass->total_score == 380);
    }
}

TEST_CASE("Tile Exchange Known", "[gcg]")
{
    SECTION(">Marlon: SEQSPO? -QO +0 268")
    {
        std::string line = ">Marlon: SEQSPO? -QO +0 268";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* xchg = std::get_if<gcg::TileExchangeKnown>(&*result);
        REQUIRE(xchg != nullptr);
        CHECK(xchg->player == "Marlon");
        CHECK(xchg->rack == "SEQSPO?");
        CHECK(xchg->tiles_exchanged == "QO");
        CHECK(xchg->total_score == 268);
    }
}
