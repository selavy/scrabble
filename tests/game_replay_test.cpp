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


std::ifstream& safe_getline(std::ifstream& ifs, std::string& line) {
    std::getline(ifs, line);
    if (!line.empty() && *line.rbegin() == '\r') {
        line.pop_back();
    }
    return ifs;
}

std::string stripline(const std::string& line)
{
    auto first = std::find_if_not(line.begin(), line.end(), isspace);
    auto rlast = std::find_if_not(line.rbegin(), line.rend(), isspace);
    auto last  = rlast.base();
    auto pos   = std::distance(line.begin(), first);
    auto count = std::distance(first, last);
    assert(0 <= pos && pos <= line.size());
    assert(0 <= count && (pos + count) <= line.size());
    using size_type = std::string::size_type;
    return line.substr(static_cast<size_type>(pos), static_cast<size_type>(count));
}

bool replay_file(std::ifstream& ifs, const Mafsa& dict)
{
    std::string line;
    while (safe_getline(ifs, line)) {
        line = stripline(line);
        std::cout << "line: \"" << line << "\"" << std::endl;
    }
    return true;
}

int main(int argc, char **argv)
{
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
    const auto maybe_dict = Mafsa::load(dictfile);
    if (!maybe_dict) {
        std::cerr << "unable to load dictionary from file: " << dictfile << std::endl;
        return 1;
    }
    const auto& dict = *maybe_dict;

    for (const auto& filename : gamefiles) {
        std::ifstream ifs{filename};
        if (!ifs) {
            fmt::print(stderr, "error: unable to open game file: {}\n", filename);
            return 1;
        }
        if (!replay_file(ifs, dict)) {
            fmt::print(stderr, "error: failed replaying game file: {}\n", filename);
            return 1;
        }
    }

    return 0;
}
