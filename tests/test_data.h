#pragma once

#include <vector>
#include <string>
#include <unordered_set>


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

inline bool isword(const std::string& s) noexcept
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
