#include <catch2/catch.hpp>
#include <mafsa/mafsa.h>
#include "test_data.h"

mafsa make_mafsa(const std::vector<std::string>& words = DICT)
{
    mafsa_builder builder;
    REQUIRE(mafsa_builder_start(&builder) == 0);
    for (const auto& word : words) {
        REQUIRE(mafsa_builder_insert(&builder, word.c_str()) == 0);
    }
    mafsa m;
    REQUIRE(mafsa_builder_finish(&builder, &m) == 0);
    return m;
}


TEST_CASE("Mafsa check word")
{
    mafsa m = make_mafsa();
    for (const auto& word : DICT) {
        CHECK(mafsa_isword(&m, word.c_str()) != 0);
    }
    for (const auto& word : MISSING) {
        CHECK(mafsa_isword(&m, word.c_str()) == 0);
    }
    mafsa_free(&m);
}
