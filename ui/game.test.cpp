#include <catch2/catch.hpp>
#include "game.h"

#if 0
    struct Play
    {
        int square;
        int direction;
        char word[]; // upper case = regular tile, lower case = blank
    };

    // TODO(peter): return score? 0 or -1 == invalid move?
    bool make_move(uint8_t* board, int sq, int dir, char word[7]);
    int  score_move(uint8_t* board, int sq, int dir, char word[7]);
#endif

TEST_CASE("Can insert words into set")
{
    std::unordered_set<Word> words;
    auto word1 = Word{"HELLO"};
    auto word2 = Word{"HELL"};
    auto word3 = Word{"HELLO"};
    auto word4 = Word{"MISSING"};
    REQUIRE(word1 != word2);
    REQUIRE(word1 == word1);
    REQUIRE(word1 == word3);
    REQUIRE(word1 != word4);
    REQUIRE(word2 != word4);

    words.insert(word1);
    words.insert(word2);
    REQUIRE(words.size() == 2u);
    words.insert(word3);
    REQUIRE(words.size() == 2u);

    SECTION("Find present word \'HELLO\'")
    {
        auto iter = words.find(word1);
        REQUIRE(iter != words.end());
        REQUIRE(*iter == word1);
    }

    SECTION("Find present word \'HELL\'")
    {
        auto word = word2;
        auto iter = words.find(word);
        REQUIRE(iter != words.end());
        REQUIRE(*iter == word);
    }

    SECTION("Don\'t find present word \'HELL\'")
    {
        REQUIRE(words.find(word4) == words.end());
    }
}


TEST_CASE("Initial Game State", "[game_state]")
{
    // auto state = make_game(/*seed*/42);
}
