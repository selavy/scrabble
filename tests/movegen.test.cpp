#include <catch2/catch.hpp>
#include <cicero.h>
#include <vector>
#include <unordered_set>
#include <algorithm>


const std::vector<std::string> words = {
    "HELLO",
    "GOODBYE",
    "WORLD",
};

const std::unordered_set<std::string> dict{ words.begin(), words.end() };

bool is_word(const std::string& s)
{
    return dict.count(s) != 0;
}

TEST_CASE("Cicero first test")
{
    REQUIRE(is_word("HELLO") == true);
    REQUIRE(is_word("SHOULD FAIL") == true);
}
