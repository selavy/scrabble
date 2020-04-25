#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <string_view>
#include <optional>

#include <fmt/format.h>
#include <cxxopts.hpp>
#include <re2/re2.h>

#include <mafsa++.h>


int main(int argc, char **argv)
{
    // clang-format off
    cxxopts::Options options(
        "integration-test",
        "replay through official games to test move generation and scoring"
    );
    options.add_options()
        ("d,dict", "dictionary file to use", cxxopts::value<std::string>(), "DICT")
        ("f,input", "input game file", cxxopts::value<std::vector<std::string>>(), "FILE")
        ;
    options.parse_positional({"dict", "input" });
    // clang-format on

    auto args = options.parse(argc, argv);
    auto gamefiles = args["input"].as<std::vector<std::string>>();
    auto dictfile  = args["dict"].as<std::string>();

    const auto maybe_dict = Mafsa::load(dictfile);
    if (!maybe_dict) {
        std::cerr << "unable to load dictionary from file: " << dictfile << std::endl;
        return 1;
    }
    const auto& dict = *maybe_dict;


    return 0;
}
