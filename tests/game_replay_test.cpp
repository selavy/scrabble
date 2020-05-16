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

#include "test_helpers.h"


re2::RE2 isc_regex(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)");
re2::RE2 move_line_regex(R"(\s*\"(.*)\", \"(.*)\"\s*)");
re2::RE2 header_regex(R"(\s*\[(\w+) \"(.*)\"]\s*)");
re2::RE2 empty_line_regex(R"(\s+)");
re2::RE2 change_line_regex(R"(\s*"?CHANGE\s+(\d+)\"?\s*)");
re2::RE2 gcg_pragma_player_regex(R"(#player(\d)\s+(\w+).*)");
re2::RE2 gcg_take_back_regex(R"(>\w+:\s+[A-Z\?]+\s+--?\s+[-+]?\d+\s+[-+]?\d+)");
re2::RE2 gcg_move_regex(R"(>(\w+):\s+([A-Z\?]+)\s+(\w+)\s+([A-Za-z\.]+)\s+([+-]?\d+)\s+([+-]?\d+).*)");
// re2::RE2 gcg_final_move_regex(R"(>(\w+):\s+\(([A-Z\.]+)\)\s+([+-]?\d+)\s+([+-]?\d+)\s*)");
re2::RE2 gcg_tile_exch_regex(R"(>(\w+):\s+([A-Z\?]+)\s+-([A-Z\?]+)\s+\+0\s+(\d+)\s*)");
re2::RE2 gcg_final_move_regex(R"(>\w+:\s+\([A-Z\?]+\)\s+[+-]?\d+\s+[+-]?\d+)");


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

enum class FileType
{
    eIsc,
    eGcg,
    eInvalid,
};

struct ReplayMove
{
    std::string         player;
    cicero_rack         rack;
    scrabble::Move      move;
    int                 change_tiles = 0; // TODO: mark which tiles changed?
};

std::optional<ReplayMove> parsemove_isc(const std::string& line, const cicero* engine)
{
    ReplayMove result;
    if (re2::RE2::FullMatch(line, change_line_regex, &result.change_tiles)) {
        return std::nullopt;
    }
    std::string rack;
    std::string isc;
    if (!re2::RE2::FullMatch(line, move_line_regex, &rack, &isc)) {
        throw std::runtime_error("invalid line in ISC file");
    }
    result.move = scrabble::Move::from_isc_spec(isc);
    cicero_make_rack(&result.rack, rack.c_str());
    return result;
}

#if 1
std::optional<ReplayMove> parsemove_gcg(const std::string& line, const cicero* engine)
{

    ReplayMove result;
    if (line[0] == '#') {
        return std::nullopt;
    } else if (line[0] != '>') {
        // fmt::print(stderr, "invalid line: \"{}\"\n", line);
        // throw std::runtime_error("invalid gcg line");

        // lot's of non-sense in cross-tables.com's inputs
        return std::nullopt;
    }

    std::string nick;
    std::string rack;
    std::string word;
    std::string sqspec; // NOTE: gcg square spec is row/col flipped from ISC
    int score;
    int total;

    if (re2::RE2::FullMatch(line, gcg_tile_exch_regex, &nick, &rack, &word, &total)) {
        fmt::print(stderr, "info: skipping tile exchange: \"{}\"\n", line);
        return std::nullopt;
    }

    if (re2::RE2::FullMatch(line, gcg_final_move_regex)) {
        fmt::print(stderr, "info: skipping final move: \"{}\"\n", line);
        return std::nullopt;
    }

    if (re2::RE2::FullMatch(line, gcg_take_back_regex)) {
        throw std::runtime_error("withdrawn moves not supported yet");
        fmt::print(stdout, "info: skipping withdrawn move: \"{}\"\n", line);
        return std::nullopt;
    }

    nick.clear();
    word.clear();
    rack.clear();
    score = -1;
    total = -1;
    if (!re2::RE2::FullMatch(line, gcg_move_regex, &nick, &rack, &sqspec, &word, &score, &total)) {
        fmt::print(stderr, "error: malformed GCG move line: \"{}\"\n", line);
        throw std::runtime_error("invalid gcg line");
    }

    cicero_make_rack(&result.rack, rack.c_str());
    auto maybe_square = scrabble::Square::from_gcg(sqspec);
    if (!maybe_square) {
        throw std::runtime_error("invalid gcg square specification");
    }
    result.move.square = *maybe_square;
    result.move.direction = scrabble::gcg_direction(sqspec);
    result.move.word = "";
    int stride = static_cast<int>(result.move.direction);
    int start  = result.move.square.value();
    int step   = 0;
    for (char ch : word) {
        if (ch == '.') {
            ch = cicero_tile_on_square(engine, start + step * stride);
        }
        result.move.word += ch;
        ++step;
    }
    result.move.score = score;
    return result;
}
#endif


bool replay_file(std::ifstream& ifs, Callbacks& cb)
{
    FileType type = FileType::eInvalid;
    // TODO: could have parse_header_gcg / parse_header_isc, but this is fine for now
    std::string line;
    while (getline_stripped(ifs, line)) {
        if (line.empty()) {
            continue;
        }
        if (line == "#pragma ISC") {
            type = FileType::eIsc;
        } else if (line == "#pragma GCG") {
            type = FileType::eGcg;
        } else {
            // fmt::print(stderr, "error: game file doesn't begin with type identifier\n");
            // return false;
            fmt::print(stdout, "warning: game file doesn't have an identifier. assuming gcg\n");
            type = FileType::eGcg;
        }
        break;
    }

    if (type == FileType::eIsc) {
        while (getline_stripped(ifs, line)) {
            if (line.empty()) {
                continue;
            }
            std::string key;
            std::string value;
            if (!re2::RE2::FullMatch(line, header_regex, &key, &value)) {
                break;
            }
            // fmt::print(stdout, "PGN Header: key=\"{}\" value=\"{}\"\n", key, value);
        }
    } else {
        getline_stripped(ifs, line);
    }

    auto* parse_fn = type == FileType::eIsc ? &parsemove_isc : &parsemove_gcg;

    cb.clear_legal_moves();
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    do {
        if (line.empty() || line[0] == '#') {
            std::cout << "skipping line: \"" << line << "\"\n";
            continue;
        }
        std::cout << "line: \"" << line << "\"" << std::endl;
        auto maybe_replay_move = parse_fn(line, &engine);
        if (!maybe_replay_move) {
            continue;
        }
        auto& replay_move = *maybe_replay_move;
        using scrabble::operator<<;
        std::cout << "\t-> " << replay_move.move << " " << replay_move.rack << "\n";

        cb.clear_legal_moves();
        cicero_generate_legal_moves(&engine, replay_move.rack);
        auto legal_moves = cb.sorted_legal_moves();

        auto match_ignoring_score = [&replay_move](const scrabble::Move& move) -> bool
        {
            return (
                replay_move.move.square == move.square &&
                replay_move.move.direction == move.direction &&
                replay_move.move.word == move.word
            );
        };

        bool move_played_in_dictionary = cb.isword(replay_move.move.word);
        if (move_played_in_dictionary) {
            auto it = std::find_if(std::begin(legal_moves), std::end(legal_moves), match_ignoring_score);
            if (it == std::end(legal_moves)) {
                std::cerr << "error: did not find played move: " << replay_move.move << ", found moves " << legal_moves << "\n";
                // std::cerr << "error: did not find played move: " << replay_move.move << "\n";
                return false;
            } else {
                std::cout << "generated move: " << replay_move.move << " correctly!\n";
            }
        } else {
            fmt::print(stderr, "\nwarning: move played that is not in dictionary: '{}'\n\n", replay_move.move.word);
        }

        // Desired API:
        //     if (cicero_legal_move(&engine, &move)) {
        //         int score = cicero_make_move(&engine, &move);
        //         // ... do something with it ...
        //         cicero_undo_move(&engine, &move);
        //     }

        auto engine_move = scrabble::EngineMove::make(&engine, replay_move.move);
        int score = cicero_make_move(&engine, &engine_move.move);
        if (score != replay_move.move.score) {
            fmt::print(stderr, "Scores don't match :( => engine={} correct={}\n\n",
                    score, replay_move.move.score);
            return false;
        } else {
            fmt::print(stdout, "Scores match! => engine={} correct={}\n\n",
                    score, replay_move.move.score);
        }

        // TEMP -- check undo_move
        cicero_undo_move(&engine, &engine.sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &engine_move.move);
        if (score != score2) {
            fmt::print(stderr, "!!! FAIL !!! after undo move scores don't match; old={} new={}\n",
                    score2, score);
            return false;
        }

    } while (getline_stripped(ifs, line));

    return true;
}

int main(int argc, char **argv)
{
    assert(isc_regex.ok());
    assert(move_line_regex.ok());
    assert(header_regex.ok());
    assert(empty_line_regex.ok());
    assert(change_line_regex.ok());
    assert(gcg_pragma_player_regex.ok());
    assert(gcg_move_regex.ok());
    assert(gcg_final_move_regex.ok());
    assert(gcg_tile_exch_regex.ok());
    assert(gcg_take_back_regex.ok());

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
        std::ifstream ifs{filename};
        if (!ifs) {
            fmt::print(stderr, "error: unable to open game file: {}\n", filename);
            return 1;
        }
        try {
            if (!replay_file(ifs, cb)) {
                fmt::print(stderr, "error: failed replaying game file: {}\n", filename);
                return 1;
            }
        } catch (...) {
            fmt::print(stderr, "Failed in: {}\n", filename);
            throw;
        }
        fmt::print(stdout, "\n\nPassed {}!\n\n", filename);
    }

    return 0;
}
