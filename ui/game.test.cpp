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

TEST_CASE("Moves tiles must be on only one row or column")
{
	Board board;

    { // H6 HORN
        GuiMove gmove = {
            { 'H', Sq_H6 },
            { 'O', Sq_H7 },
            { 'R', Sq_H8 },
            { 'N', Sq_H9 },
        };
        std::cerr << "BEFORE:\n" << board << std::endl;
        auto maybe_move = make_move(board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == Player::Player1);
        CHECK(move.score == 14);
        CHECK(move.square == Sq_H6);
        CHECK(move.direction == Direction::HORIZONTAL);
        CHECK(move.length == 4);
        CHECK(tiles_have_word(move.tiles, "HORN"));
        std::cerr << "AFTER:\n" << board << "\n" << std::endl;
    }

    { // F8 FARM
        GuiMove gmove = {
            { 'F', Sq_F8 },
            { 'A', Sq_G8 },
            // { 'R', Sq_H8 },
            { 'M', Sq_I8 },
        };
        std::cerr << "BEFORE:\n" << board << std::endl;
        auto maybe_move = make_move(board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == Player::Player2);
        CHECK(move.score == 9);
        CHECK(move.square == Sq_F8);
        CHECK(move.direction == Direction::VERTICAL);
        CHECK(move.length == 4);
        CHECK(tiles_have_word(move.tiles, "FAM"));
        std::cerr << "AFTER:\n" << board << "\n" << std::endl;
    }
}



#if 0
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

        // TODO: will have to fix this because words not in dictionary
        {
            GuiMove gmove = {
                // { 'L', Sq_I9 },
                { 'I', Sq_I10 },
                { 'G', Sq_I11 },
                { 'H', Sq_I12 },
                { 'T', Sq_I13 },
                { 'S', Sq_I14 },
            };
            std::cerr << board << std::endl;
            auto maybe_move = Move::make(board, gmove);
            REQUIRE(static_cast<bool>(maybe_move) == true);
            auto move = *maybe_move;
            REQUIRE(move.direction == Direction::HORIZONTAL);
            REQUIRE(move.square == Sq_I9);
            REQUIRE(move.max_word_length == 6);
            make_move(board, move);
            std::cerr << board << std::endl;
        }

        { // not valid because no contiguous tiles
            GuiMove gmove = {
                { 'P', Sq_E14 },
                { 'E', Sq_F14 },
                { 'T', Sq_G14 },
            };
            auto maybe_move = Move::make(board, gmove);
            REQUIRE(static_cast<bool>(maybe_move) == false);
        }

        //-------------------------------------------------------------------
        //
		// !!!! Working on this case: !!!!
        //
		//      1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
		//    -------------------------------------------------------------
		// A  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// B  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// C  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// D  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// E  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// F  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// G  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// H  |   |   |   |   |   |   |   | L | O | V | E |   |   |   |   |
		//    -------------------------------------------------------------
		// I  |   |   |   |   |   |   |   |   | L | I | G | H | T | S |   |
		//    -------------------------------------------------------------
		// J  |   |   |   |   |   |   |   |   | I |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// K  |   |   |   |   |   |   |   |   | V |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// L  |   |   |   |   |   |   |   |   | E |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// M  |   |   | l | e | t | t | e | r | s |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// N  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		// O  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
		//    -------------------------------------------------------------
		//
        // When playing "letters" connected to "olive" (forming "olives") need to
		// say that word direction = HORZ and length = 7.
		//
        //-------------------------------------------------------------------
        {
            GuiMove gmove = {
                { 'L', Sq_M3 },
                { 'E', Sq_M4 },
                { 'T', Sq_M5 },
                { 'T', Sq_M6 },
                { 'E', Sq_M7 },
                { 'R', Sq_M8 },
                { 'S', Sq_M9 }, // forms OLIVEs
            };
            std::cerr << board << std::endl;
            auto maybe_move = Move::make(board, gmove);
            REQUIRE(static_cast<bool>(maybe_move) == true);
            auto move = *maybe_move;
            REQUIRE(move.direction == Direction::HORIZONTAL);
            REQUIRE(move.square == Sq_M3);
            REQUIRE(move.max_word_length == 7);
            make_move(board, move);
            std::cerr << board << std::endl;
        }
    }
}
#endif

TEST_CASE("Initial Game State", "[game_state]")
{
    // auto state = make_game(/*seed*/42);
}
