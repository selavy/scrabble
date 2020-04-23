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
        INFO("Checking " << word);
        CHECK(mafsa_isword(&m, word.c_str()) != 0);
    }
    for (const auto& word : MISSING) {
        CHECK(mafsa_isword(&m, word.c_str()) == 0);
    }
    mafsa_free(&m);
}

TEST_CASE("Mafsa edges")
{
    const std::vector<std::string> words = {
        "ABA",
        "ABAS",
        "ABAAAAAS",
        "ABB",
        "ABD",
        "ABZ",
        "GOODBYE",
        "AWORD",
        "ACA",
        "GOO",
        "BOO",
    };
    mafsa m = make_mafsa(words);

    SECTION("Prefix AB has several edges")
    {
        mafsa_edges es = mafsa_prefix_edges(&m, "AB");
        CHECK(es.terminal == false);
        CHECK(es.edges[0] == 'A');
        CHECK(es.edges[1] == 'B');
        CHECK(es.edges[2] == 'D');
        CHECK(es.edges[3] == 'Z');
        CHECK(es.edges[4] == '\0');
    }

    SECTION("Prefix ABE has no edges")
    {
        const char *prefix = "ABE";
        REQUIRE(mafsa_isword(&m, prefix) == false);
        mafsa_edges es = mafsa_prefix_edges(&m, prefix);
        CHECK(es.terminal == false);
        CHECK(es.edges[0] == '\0');
    }

    SECTION("Prefix ABA has edges and is terminal")
    {
        const char *prefix = "ABA";
        REQUIRE(mafsa_isword(&m, prefix) == true);
        mafsa_edges es = mafsa_prefix_edges(&m, prefix);
        CHECK(es.terminal == true);
        CHECK(es.edges[0] == 'A');
        CHECK(es.edges[1] == 'S');
        CHECK(es.edges[2] == '\0');
    }

    mafsa_free(&m);
}
