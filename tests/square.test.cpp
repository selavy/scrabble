#include <catch2/catch.hpp>
#include "square.h"


TEST_CASE("Equality", "[square]")
{
    constexpr auto sq1 = *hume::Square::make(43);
    constexpr auto sq2 = *hume::Square::make(43);
    REQUIRE(sq1 == sq2);
}

TEST_CASE("Name", "[square]")
{
    SECTION("A1")
    {
        constexpr auto square = hume::Square::make(0);
        REQUIRE(square.has_value());
        CHECK(square->name() == " A1");
    }

    SECTION("O15")
    {
        constexpr auto square = hume::Square::make(224);
        REQUIRE(square.has_value());
        CHECK(square->name() == "O15");
    }

    SECTION("H8")
    {
        constexpr auto square = hume::Square::make(112);
        REQUIRE(square.has_value());
        CHECK(square->name() == " H8");
    }
}

TEST_CASE("Column and Row", "[square]")
{
    auto colnum = [](char c) -> int
    {
        assert('A' <= c && c <= 'O');
        return c - 'A';
    };

    auto rownum = [](int r) -> int
    {
        assert(1 <= r && r <= 15);
        return r - 1;
    };

    SECTION("A1")
    {
        constexpr auto square = hume::Square::make(0);
        REQUIRE(square.has_value());
        CHECK(square->col() == colnum('A'));
        CHECK(square->row() == rownum(1));
    }

    SECTION("O15")
    {
        constexpr auto square = hume::Square::make(224);
        REQUIRE(square.has_value());
        CHECK(square->col() == colnum('O'));
        CHECK(square->row() == rownum(15));
    }

    SECTION("H8")
    {
        constexpr auto square = hume::Square::make(112);
        REQUIRE(square.has_value());
        CHECK(square->col() == colnum('H'));
        CHECK(square->row() == rownum(8));
    }
}
