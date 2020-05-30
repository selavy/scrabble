#include <catch2/catch.hpp>
#include "gcg.h"

using namespace scrabble;

TEST_CASE("Parse pragmata")
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
