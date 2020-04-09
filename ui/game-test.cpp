#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <fmt/format.h>
#include <cxxopts.hpp>
#include <re2/re2.h>
#include "game.h"
#include "engine.h"

std::optional<EngineTrie> load_dictionary(std::string path)
{
    EngineTrie dict;
    std::string   word;
    std::ifstream ifs{path};
    if (!ifs) {
        std::cerr << "error: unable to open input file\n";
        return std::nullopt;
    }
    while (ifs >> word) {
        if (word.empty()) {
            continue;
        }
        if (word.size() < 2 || word.size() > 15) {
            std::cerr << "warning: skipping invalid word: \"" << word << "\"\n";
        }
        bool valid_word = true;
        for (std::size_t i = 0; i < word.size(); ++i) {
            char c = word[i];
            if ('a' <= c && c <= 'z') {
                word[i] = (c - 'a') + 'A';
            } else if ('A' <= c && c <= 'Z') {
                word[i] = c;
            } else {
                std::cerr << "warning: invalid character '" << c << "' in word \"" << word << "\"\n";
                valid_word = false;
                break;
            }
        }
        if (valid_word) {
            dict.insert(word);
        }
    }
    return dict;
}

void replay_game(std::ifstream& ifs, const EngineTrie& dict)
{

}

int main(int argc, char** argv)
{
    cxxopts::Options options("game-test", "replay through official scrabble games to test engine");
    options.add_options()
        ("d,dict",  "dictionary file to use", cxxopts::value<std::string>(), "DICT")
        ("f,input", "input game file",        cxxopts::value<std::string>(), "FILE")
        ;
    options.parse_positional({ "dict", "input", "too many positional arguments" });
    auto args = options.parse(argc, argv);

    if (args.count("help")) {
        std::cerr << options.help() << std::endl;
        return 0;
    }
    if (!args.count("dict")) {
        fmt::print(stderr, "error: must specify dictionary file to use.\n");
        return 1;
    }
    if (!args.count("input")) {
        fmt::print(stderr, "error: must specify input file to replay.\n");
        return 1;
    }

    auto gamefile = args["input"].as<std::string>();
    auto dictfile = args["dict" ].as<std::string>();
    fmt::print(stdout, "Game File: \"{}\"\n", gamefile);
    fmt::print(stdout, "Dict File: \"{}\"\n", dictfile);

    auto maybe_dict = load_dictionary(dictfile);
    if (!maybe_dict) {
        fmt::print(stderr, "error: unable to load dictionary from file: \"{}\"\n", dictfile);
        return 1;
    }
    const auto& dict = *maybe_dict;

    fmt::print(stdout, "Replaying \"{}\"\n", gamefile);
    std::ifstream ifs{gamefile};
    if (!ifs) {
        fmt::print(stderr, "error: unable to open game file: \"{}\"\n", gamefile);
        return 1;
    }
    replay_game(ifs, dict);

    return 0;
}
