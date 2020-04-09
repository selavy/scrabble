#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <cassert>
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

re2::RE2 isc_regex(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)");
re2::RE2 move_line_regex(R"(\s*\"(.*)\", \"(.*)\"\s*)");
re2::RE2 header_regex(R"(\s*\[(\w+) \"(.*)\"]\s*)");
re2::RE2 empty_line_regex(R"(\s+)");
re2::RE2 change_line_regex(R"(\s*"?CHANGE\s+(\d+)\"?\s*)");

bool is_empty_line(const std::string& line)
{
    return line.empty() || re2::RE2::FullMatch(line, empty_line_regex);
}

bool replay_game(std::ifstream& ifs, const EngineTrie& dict)
{
    assert(isc_regex.ok());
    assert(move_line_regex.ok());
    assert(header_regex.ok());
    assert(empty_line_regex.ok());
    assert(change_line_regex.ok());

    std::string line;
    while (std::getline(ifs, line)) {
        if (is_empty_line(line)) {
            // fmt::print(stdout, "Skipping blank line: \"{}\"\n", line);
            continue;
        }
        std::string key;
        std::string value;
        if (!re2::RE2::FullMatch(line, header_regex, &key, &value)) {
            fmt::print(stdout, "Found first non-header line: \"{}\"\n", line);
            break;
        }
        fmt::print(stdout, "PGN Header: key=\"{}\" value=\"{}\"\n", key, value);
    }

    do {
        if (is_empty_line(line)) {
            // fmt::print(stdout, "Skipping blank line: \"{}\"\n", line);
            continue;
        }

        int change_num_tiles = 0;
        if (re2::RE2::FullMatch(line, change_line_regex, &change_num_tiles)) {
            fmt::print(stdout, "Change {} tiles\n", change_num_tiles);
            continue;
        }

        std::string rack_spec;
        std::string isc_spec;
        if (!re2::RE2::FullMatch(line, move_line_regex, &rack_spec, &isc_spec)) {
            fmt::print(stderr, "error: invalid move: \"{}\"\n", line);
            return false;
        }

        IscMove move;
        move.score = -1;
        re2::RE2::FullMatch(isc_spec, isc_regex, &move.sqspec, &move.root, &move.score);
        if (move.sqspec.empty() || move.root.empty()) {
            fmt::print(stderr, "error: invalid ISC move: \"{}\"\n", isc_spec);
            return false;
        }

        std::cout << "Parsed ISC move: " << move << "\n";

    } while (std::getline(ifs, line));

    return true;
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
    if (!replay_game(ifs, dict)) {
        fmt::print(stderr, "error: unable to replay game file: \"{}\"\n", gamefile);
        return 1;
    }

    return 0;
}
