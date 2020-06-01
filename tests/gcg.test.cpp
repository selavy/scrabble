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
        // CHECK(move->word   == "anOther");
        CHECK(move->word   == "ANoTHER");
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

TEST_CASE("Tile Exchange Count", "[gcg]")
{
    SECTION(">Marlon: SEQSPO? -2 +0 268")
    {
        std::string line = ">Marlon: SEQSPO? -2 +0 268";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* xchg = std::get_if<gcg::TileExchangeCount>(&*result);
        REQUIRE(xchg != nullptr);
        CHECK(xchg->player == "Marlon");
        CHECK(xchg->rack == "SEQSPO?");
        CHECK(xchg->num_tiles == 2);
        CHECK(xchg->total_score == 268);
    }
}

TEST_CASE("Phoney Removed", "[gcg]")
{
    SECTION(">Ron: MOULAGD -- -76 354")
    {
        std::string line = ">Ron: MOULAGD -- -76 354";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* move = std::get_if<gcg::PhoneyRemoved>(&*result);
        REQUIRE(move != nullptr);
        CHECK(move->player == "Ron");
        CHECK(move->rack == "MOULAGD");
        CHECK(move->score == -76);
        CHECK(move->total_score == 354);
    }
}

TEST_CASE("Challenge Bonus", "[gcg]")
{
    SECTION(">Joel: DROWNUG (challenge) +5 289")
    {
        std::string line = ">Joel: DROWNUG (challenge) +5 289";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* move = std::get_if<gcg::ChallengeBonus>(&*result);
        REQUIRE(move != nullptr);
        CHECK(move->player == "Joel");
        CHECK(move->rack == "DROWNUG");
        CHECK(move->bonus == 5);
        CHECK(move->total_score == 289);
    }
}

TEST_CASE("Last Rack Points", "[gcg]")
{
    SECTION(">Dave: (G) +4 539")
    {
        std::string line = ">Dave: (G) +4 539";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* move = std::get_if<gcg::LastRackPoints>(&*result);
        REQUIRE(move != nullptr);
        CHECK(move->player == "Dave");
        CHECK(move->rack == "G");
        CHECK(move->score == 4);
        CHECK(move->total_score == 539);
    }
}

TEST_CASE("Time Penalty", "[gcg]")
{
    SECTION(">Pakorn: ISBALI (time) -10 409")
    {
        std::string line = ">Pakorn: ISBALI (time) -10 409";
        gcg::Parser p;
        auto result = p.parse_line(line);
        REQUIRE(result.has_value());
        auto* pen = std::get_if<gcg::TimePenalty>(&*result);
        REQUIRE(pen != nullptr);
        CHECK(pen->player == "Pakorn");
        CHECK(pen->rack == "ISBALI");
        CHECK(pen->penalty == -10);
        CHECK(pen->total_score == 409);
    }
}

