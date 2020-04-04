#include <catch2/catch.hpp>
#include "game.h"
#include <iostream>


TEST_CASE("Can insert words into set")
{
    for (int i = 0; i < MaxWordLength; ++i) {
        REQUIRE(all_empty_tiles[i] == Empty);
    }

    Dictionary dict;
    auto word1 = Word{"HELLO"};
    auto word2 = Word{"HELL"};
    auto word3 = Word{"HELLO"};
    auto word4 = Word{"MISSING"};
    REQUIRE(word1 != word2);
    REQUIRE(word1 == word1);
    REQUIRE(word1 == word3);
    REQUIRE(word1 != word4);
    REQUIRE(word2 != word4);

    dict.insert(word1);
    dict.insert(word2);
    REQUIRE(dict.size() == 2u);
    dict.insert(word3);
    REQUIRE(dict.size() == 2u);

    SECTION("Find present word \'HELLO\'")
    {
        auto iter = dict.find(word1);
        REQUIRE(iter != dict.end());
        REQUIRE(*iter == word1);
    }

    SECTION("Find present word \'HELL\'")
    {
        auto word = word2;
        auto iter = dict.find(word);
        REQUIRE(iter != dict.end());
        REQUIRE(*iter == word);
    }

    SECTION("Don\'t find present word \'HELL\'")
    {
        REQUIRE(dict.find(word4) == dict.end());
    }
}

#define AsInt(x) static_cast<int>(x)

TEST_CASE("Create move guarantees contiguous in 1 direction")
{
    {
        Board board;

        {
            GuiMove gmove = {
                { 'L', Sq_H8  },
                { 'O', Sq_H9  },
                { 'V', Sq_H10 },
                { 'E', Sq_H11 },
            };
            std::cerr << board << std::endl;
            auto maybe_move = Move::make(board, gmove);
            REQUIRE(static_cast<bool>(maybe_move) == true);
            auto move = *maybe_move;
            REQUIRE(move.direction == Direction::HORIZONTAL);
            REQUIRE(move.square == Sq_H8);
            REQUIRE(move.max_word_length == 4);
            make_move(board, move);
            std::cerr << board << std::endl;
        }

        {
            GuiMove gmove = {
                // { 'O', Sq_H9 },
                { 'L', Sq_I9 },
                { 'I', Sq_J9 },
                { 'V', Sq_K9 },
                { 'E', Sq_L9 },
            };
            std::cerr << board << std::endl;
            auto maybe_move = Move::make(board, gmove);
            REQUIRE(static_cast<bool>(maybe_move) == true);
            auto move = *maybe_move;
            REQUIRE(move.direction == Direction::VERTICAL);
            REQUIRE(move.square == Sq_H9);
            REQUIRE(move.max_word_length == 5);
            make_move(board, move);
            std::cerr << board << std::endl;
        }
    }
}


TEST_CASE("Initial Game State", "[game_state]")
{
    // auto state = make_game(/*seed*/42);
}
