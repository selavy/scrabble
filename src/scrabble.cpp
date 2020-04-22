#include <cstdio>
#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include "mafsa.h"

int main(int argc, const char** argv) {
    if (argc <= 1) {
        std::cout << "usage: " << argv[0] << " [DICT]" << std::endl;
        return 0;
    }

    auto maybe_dict = Mafsa::load(argv[1]);
    if (!maybe_dict) {
        std::cerr << "error: unable to load dictionary from: " << argv[1] << "\n";
        return 1;
    }
    const auto& dict = *maybe_dict;


    const std::vector<std::string> words = {
        "AA",
        "AAH",
        "ABACTERIAL",
        "ASDF",
    };

    for (const auto& word : words) {
        std::cout << word << " is a word? " << (dict.isword(word) ? "TRUE" : "FALSE") << "\n";
    }

    return 0;
}
