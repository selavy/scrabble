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

std::ifstream& safe_getline(std::ifstream& ifs, std::string& line) {
    std::getline(ifs, line);
    line = stripline(line);
    return ifs;
}

enum class FileType
{
    eIsc,
    eGcg,
    eInvalid,
};

struct ReplayMove
{
    using scrabble::Direction;

    std::string         player;
    std::array<char, 7> rack;       // ' ' == blank, '?' == unknown
    int                 square;
    Direction           direction;
    std::string         word;       // uppercase = regular tile, lowercase = blank
    int                 score = -1;

    int                 change_tiles = 0; // TODO: mark which tiles changed?
};

re2::RE2 isc_regex(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)");
re2::RE2 move_line_regex(R"(\s*\"(.*)\", \"(.*)\"\s*)");
re2::RE2 header_regex(R"(\s*\[(\w+) \"(.*)\"]\s*)");
re2::RE2 empty_line_regex(R"(\s+)");
re2::RE2 change_line_regex(R"(\s*"?CHANGE\s+(\d+)\"?\s*)");

std::optional<ReplayMove> parsemove_isc(const std::string& line)
{
    assert(isc_regex.ok());
    assert(move_line_regex.ok());
    assert(header_regex.ok());
    assert(empty_line_regex.ok());
    assert(change_line_regex.ok());

    ReplayMove result;
    if (re2::RE2::FullMatch(line, change_line_regex, &result.change_tiles)) {
        return std::nullopt;
    }

    std::string rack;
    std::string isc;
    if (!re2::RE2::FullMatch(line, move_line_regex, &rack, &isc)) {
        throw std::runtime_error("invalid line in ISC file");
    }

    auto move = scrabble::Move::from_isc_spec(isc);
    result.square    = move.square;
    result.direction = move.direction;
    result.word      = move.word;
    result.score     = move.score;

    // std::fill(std::begin(result.rack), std::end(result.rack), '?');
    // for (const char c : rack) {

    // }
    return result;
}

bool replay_file(std::ifstream& ifs, const Mafsa& dict)
{
    FileType type = FileType::eInvalid;

    std::string line;
    while (safe_getline(ifs, line)) {
        if (line.empty()) {
            continue;
        }
        if (line == "#pragma ISC") {
            type = FileType::eIsc;
        } else if (line == "#pragma GCG") {
            type = FileType::eGcg;
        } else {
            fmt::print(stderr, "error: game file doesn't begin with type identifier");
            return false;
        }
        break;
    }

    while (safe_getline(ifs, line)) {
        line = stripline(line);
        if (line.empty() || line[0] == '#') {
            std::cout << "skipping line: \"" << line << "\"\n";
            continue;
        }



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
