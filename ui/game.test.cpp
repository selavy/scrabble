#include <catch2/catch.hpp>
#include "game.h"
#include <iostream>


TEST_CASE("Can insert words into set")
{
    for (std::size_t i = 0; i < all_empty_tiles.size(); ++i) {
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

bool tiles_have_word(const Tiles& tile, std::string word)
{
    assert(word.size() <= tile.size());
    size_t i = 0;
    for (i = 0; i < word.size(); ++i) {
        if (word[i] != tile[i]) {
            return false;
        }
    }
    for (; i < tile.size(); ++i) {
        if (tile[i] != Empty) {
            return false;
        }
    }
    return true;
}

TEST_CASE("Scrabble rules example", "[move_validation]")
{
	Board board;

    std::vector<std::tuple<GuiMove, Player, Score, Square, Direction, int, std::string, std::string>> ts = {
        {
            {
                { 'H', Sq_H6 },
                { 'O', Sq_H7 },
                { 'R', Sq_H8 },
                { 'N', Sq_H9 },
            },
            Player::Player1, 14, Sq_H6, Direction::HORIZONTAL, 4, "HORN", "HORN"
        },
        {
            {
                { 'F', Sq_F8 },
                { 'A', Sq_G8 },
                // { 'R', Sq_H8 },
                { 'M', Sq_I8 },
            },
            Player::Player2, 9, Sq_F8, Direction::VERTICAL, 4, "FAM", "FARM"
        },
        {
            {
                { 'P', Sq_J6 },
                { 'A', Sq_J7 },
                { 'S', Sq_J8 },
                { 'T', Sq_J9 },
                { 'E', Sq_J10 },
            },
            Player::Player1, 25, Sq_J6, Direction::HORIZONTAL, 5, "PASTE", "PASTE"
        },
        {
            {
                // { 'M', Sq_I8 },
                { 'O', Sq_I9 },
                { 'B', Sq_I10 },
            },
            Player::Player2, 16, Sq_I8, Direction::HORIZONTAL, 3, "OB", "MOB"
        },
        {
            {
                { 'B', Sq_K5 },
                { 'I', Sq_K6 },
                { 'T', Sq_K7 },
            },
            Player::Player1, 16, Sq_K5, Direction::HORIZONTAL, 3, "BIT", "BIT"
        },
    };

    for (auto&& [gmove, player, score, square, direction, length, tiles, root_word] : ts) {
        // std::cerr << "BEFORE:\n" << board << std::endl;
        auto maybe_move = make_move(board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        CHECK(tiles_have_word(move.tiles, tiles));
        CHECK(move.root_word == root_word);
        // std::cerr << "AFTER:\n" << board << "\n" << std::endl;

    }
}

TEST_CASE("selavy v andybfan example")
{
    Board board;

    std::vector<std::tuple<GuiMove, Player, Score, Square, Direction, int>> moves =
    {
        {
            {
                { 'S', Sq_H8 },
                { 'E', Sq_H9 },
                { 'E', Sq_H10 },
            },
            Player::Player1, 6, Sq_H8, Direction::HORIZONTAL, 3
        },
        {
            {
                { 'M', Sq_H11 },
                { 'A', Sq_I11 },
                { 'Y', Sq_J11 },
                { 'O', Sq_K11 },
            },
            Player::Player2, 24, Sq_H11, Direction::VERTICAL, 4
        },
        {
            {
                { 'F', Sq_J9  },
                { 'L', Sq_J10 },
                // { 'Y', Sq_J11 },
            },
            Player::Player1, 8, Sq_J9, Direction::HORIZONTAL, 3
        },
        {
            {
                { 'R', Sq_K12 },
                { 'A', Sq_L12 },
                { 'G', Sq_M12 },
                { 'E', Sq_N12 },
            },
            Player::Player2, 12, Sq_K12, Direction::VERTICAL, 4
        },
        {
            {
                // { 'G', Sq_M12 },
                { 'O', Sq_M13 },
            },
            Player::Player1, 6, Sq_M12, Direction::HORIZONTAL, 6
        },
    };

    for (auto&& [gmove, player, score, square, direction, length] : moves) {
        // std::cerr << "BEFORE:\n" << board << std::endl;
        auto maybe_move = make_move(board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        // std::cerr << "AFTER:\n" << board << "\n" << std::endl;
    }
}
