#include <cstdio>
#include <cassert>
#include <vector>
#include <string>
#include <unordered_set>
#include "mafsa.h"

// clang-format off
const std::vector<std::string> DICT = {
    "AA",
    "AAH",
    "AAHED",
    "AAHING",
    "AAHS",
    "AAL",
    "AALII",
    "AALIIS",
    "AALS",
    "AARDVARK",
    "AARDVARKS",
    "AARDWOLF",
    "AARDWOLVES",
    "CEANOTHUS",
    "CEANOTHUSES",
    "CEAS",
    "CEASE",
    "CEASED",
    "CEASEFIRE",
    "CEASEFIRES",
    "CEASELESS",
    "MIDWIVING",
    "MIDYEAR",
    "MIDYEARS",
    "MIELIE",
    "MIELIES",
    "GAUJES",
    "GAULEITER",
    "GAULEITERS",
    "THEATERLANDS",
    "THEATERS",
    "THEATRAL",
    "TYPEY",
    "TYPHACEOUS",
    "TYPHLITIC",
    "TYPHLITIS",
    "UPDATING",
    "UPDIVE",
    "UPDIVED",
    "VICTIMIZE",
    "VOICER",
    "VOICERS",
    "VOICES",
    "VOICING",
    "WILDCATTINGS",
    "WILDEBEEST",
    "WILDEBEESTS",
    "WILDED",
    "WOOFTAH",
    "WOOFTAHS",
    "WOOFTER",
    "YAMPY",
    "YAMS",
    "YAMULKA",
    "ZYMOSANS",
    "ZYMOSES",
    "ZYMOSIMETER",
    "ZYMOSIMETERS",
    "ZYMOSIS",
};
// clang-format on

const std::unordered_set<std::string> WORDSET{DICT.begin(), DICT.end()};

bool isword(const std::string& s) noexcept
{
    return WORDSET.count(s) != 0;
}

// clang-format off
const std::vector<std::string> MISSING = {
    "ABIDDEN",
    "ABIDE",
    "ABIDED",
    "ABIDER",
    "ACCOUSTREMENTS",
    "ACCOUTER",
    "ACCOUTERED",
    "FUTURISTIC",
    "FUTURISTICALLY",
    "FUTURISTICS",
    "XIPHOIDAL",
    "XIPHOIDS",
    "XIPHOPAGI",
    "ZZZ",
    "ZAZAZ",
    "AAAA",
    "AZZZZ",
    "YZY",

    "AALA",
    "AALB",
    "AALC",
    "AALD",
    "AALE",
    "AALF",
    "AALG",
    "AALH",
    "AALI",
    "AALJ",
    "AALK",
    "AALL",
    "AALM",
    "AALN",
    "AALO",
    "AALP",
    "AALQ",
    "AALR",
    "AALT",
    "AALU",
    "AALV",
    "AALW",
    "AALX",
    "AALY",
    "AALZ",

    "YAMA",
    "YAMB",
    "YAMC",
    "YAMD",
    "YAME",
    "YAMF",
    "YAMG",
    "YAMH",
    "YAMI",
    "YAMJ",
    "YAMK",
    "YAML",
    "YAMM",
    "YAMN",
    "YAMO",
    "YAMP",
    "YAMQ",
    "YAMR",
    "YAMT",
    "YAMU",
    "YAMV",
    "YAMW",
    "YAMZ",

    "MIDYA",
    "MIDYB",
    "MIDYC",
    "MIDYD",
    "MIDYF",
    "MIDYG",
    "MIDYH",
    "MIDYI",
    "MIDYJ",
    "MIDYK",
    "MIDYL",
    "MIDYM",
    "MIDYN",
    "MIDYO",
    "MIDYP",
    "MIDYQ",
    "MIDYR",
    "MIDYS",
    "MIDYT",
    "MIDYU",
    "MIDYV",
    "MIDYW",
    "MIDYX",
    "MIDYY",
    "MIDYZ",
};
// clang-format on

// #define TRY(x) if ((rc = (x)) != 0)

int main([[maybe_unused]]int argc, [[maybe_unused]] const char** argv) {
    int rc;
    mafsa_builder builder;

    rc = mafsa_builder_start(&builder);
    if (rc != 0) {
        fprintf(stderr, "error: failed to initialize builder\n");
        return 1;
    }

    for (const auto& word : DICT) {
        rc = mafsa_builder_insert(&builder, word.c_str());
        if (rc != 0) {
            fprintf(stderr, "error: failed to insert word: '%s'\n", word.c_str());
            return 1;
        }
    }

    mafsa m;
    rc = mafsa_builder_finish(&builder, &m);
    if (rc != 0) {
        fprintf(stderr, "error: failed to finalize builder\n");
        return 1;
    }

    int tests = 0;
    int fails = 0;
    for (const auto& word : DICT) {
        if (mafsa_isword(&m, word.c_str()) != true) {
            printf("FAILED: '%s' expect=true actual=false\n", word.c_str());
            ++fails;
        }
        ++tests;
    }

    for (const auto& word : MISSING) {
        if (mafsa_isword(&m, word.c_str()) != false) {
            printf("FAILED: '%s' expect=false actual=true\n", word.c_str());
            ++fails;
        }
        ++tests;
    }

    printf("# pass = %d; # fails = %d; # tests = %d\n", tests-fails, fails, tests);
    if (fails == 0) {
        printf("passed.\n");
    }

    mafsa_free(&m);

    return 0;
}
