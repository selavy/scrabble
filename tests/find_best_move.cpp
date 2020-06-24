#include <algorithm>
#include <fstream>
#include <optional>
#include <iostream>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <cxxopts.hpp>
#include <re2/re2.h>

#include <mafsa++.h>
#include <scrabble.h>
#include <gcg.h>

#include "test_helpers.h"


std::string stripline(const std::string& line)
{
    auto first = std::find_if_not(line.begin(), line.end(), isspace);
    auto rlast = std::find_if_not(line.rbegin(), line.rend(), isspace);
    auto last  = rlast.base();
    auto pos   = std::distance(line.begin(), first);
    auto count = std::distance(first, last);
    assert(0 <= pos && pos <= line.size());
    assert(0 <= (pos + count) && (pos + count) <= line.size());
    using size_type = std::string::size_type;
    return line.substr(static_cast<size_type>(pos), static_cast<size_type>(count));
}

std::ifstream& getline_stripped(std::ifstream& ifs, std::string& line) {
    std::getline(ifs, line);
    line = stripline(line);
    return ifs;
}

bool starts_with(const std::string& s, std::string_view sv) noexcept
{
    // return std::string_view{s}.starts_with(sv);
    return std::string_view{s}.substr(0, sv.size()) == sv;
}

void run(std::ifstream& ifs, Callbacks& cb)
{
    cicero_savepos sp;
    cicero engine;
    cicero_init_wwf(&engine, cb.make_callbacks());
    std::string line;
    while (getline_stripped(ifs, line)) {
        if (line.empty()) {
            continue;
        }
        // if (starts_with(line, "#rack")) {
        //     break;
        // }
        auto move  = scrabble::Move::from_isc_spec(line);
        auto emove = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_make_move(&engine, &sp, &emove.move);
        std::cout << move << " scored " << score << "\n";
        std::cout << engine << "\n\n";
    }

    // if (!getline_stripped(ifs, line)) {
    //     throw std::runtime_error("expecting rack")
    // }

    const char* tiles = "IAAREJL";
    cicero_rack rack;
    cicero_make_rack(&rack, tiles);
    cb.clear_legal_moves();
    cicero_generate_legal_moves(&engine, rack);
    auto moves = cb.sorted_legal_moves();
    int i = 0;

    for (auto& move : moves) {
        auto emove = scrabble::EngineMove::make(&engine, move);
        auto score = cicero_score_move(&engine, &emove.move);
        move.score = score;
    }

    std::sort(moves.begin(), moves.end(), [](const auto& m1, const auto& m2) {
            return m1.score > m2.score; });

    for (const auto& move : moves) {
        std::cout << move << "\n";
        ++i;
        if (i > 30) {
            break;
        }
    }
}

int main(int argc, char** argv)
{
    using scrabble::operator<<;

    // clang-format off
    cxxopts::Options options(
        "integration-test",
        "replay through official games to test move generation and scoring"
    );
    options.add_options()
        ("d,dict", "dictionary file to use", cxxopts::value<std::string>(), "DICT")
        ("f,file", "input game file", cxxopts::value<std::vector<std::string>>(), "FILE")
        ("h,help", "print usage")
        ;
    options.positional_help("[FILE...]");
    options.parse_positional({"file"});
    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    // clang-format on

    auto gamefiles = args["file"].as<std::vector<std::string>>();
    auto dictfile  = args["dict"].as<std::string>();
    auto maybe_dict = Mafsa::load(dictfile);
    if (!maybe_dict) {
        std::cerr << "unable to load dictionary from file: " << dictfile << std::endl;
        return 1;
    }
    auto& dict = *maybe_dict;
    auto cb = Callbacks{std::move(dict)};
    for (const auto& filename : gamefiles) {
        fmt::print(stdout, "replaying '{}'\n", filename);
        std::ifstream ifs{filename};
        if (!ifs) {
            fmt::print(stderr, "error: unable to open game file: {}\n", filename);
            continue;
        }
        run(ifs, cb);
    }

    // const std::vector<std::string> moves = {
    //     "H4 drivel",
    //     "4F old",
    //     "5F fard",
    //     "6G xi",
    //     "8E ashen",
    // };


    return 0;
}
