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


re2::RE2 isc_regex(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)");
re2::RE2 move_line_regex(R"(\s*\"(.*)\", \"(.*)\"\s*)");
re2::RE2 header_regex(R"(\s*\[(\w+) \"(.*)\"]\s*)");
re2::RE2 empty_line_regex(R"(\s+)");
re2::RE2 change_line_regex(R"(\s*"?CHANGE\s+(\d+)\"?\s*)");


struct Callbacks
{
    using Move = scrabble::Move;

    Callbacks(const Mafsa* m) noexcept : mafsa_{m}, legal_moves_{} {}

    static cicero_edges prefix_edges(void* data, const char* prefix) noexcept
    {
        auto* self = reinterpret_cast<const Callbacks*>(data);
        return self->prefix_edges_(prefix);
    }

    static void on_legal_move(void* data, const char* word, int square, int direction) noexcept
    {
        auto* self = reinterpret_cast<Callbacks*>(data);
        return self->on_legal_move_(word, square, direction);
    }

    void clear_legal_moves() noexcept
    {
        legal_moves_.clear();
    }

    std::vector<Move> sorted_legal_moves() const noexcept
    {
        auto result = legal_moves_;
        std::sort(result.begin(), result.end());
        return result;
    }

    cicero_callbacks make_callbacks() const noexcept
    {
        cicero_callbacks cb;
        cb.onlegal = &Callbacks::on_legal_move;
        cb.getedges = &Callbacks::prefix_edges;
        cb.onlegaldata = this;
        cb.getedgesdata = this;
        return cb;
    }

private:
    cicero_edges prefix_edges_(const char* prefix) const noexcept
    {
        cicero_edges out;
        auto edges = mafsa_->get_edges(prefix);
        static_assert(sizeof(edges) == sizeof(out));
        memcpy(&out, &edges, sizeof(out));
        return out;
    }

    void on_legal_move_(const char* word, int square, int direction)
    {
        legal_moves_.emplace_back();
        legal_moves_.back().square = square;
        legal_moves_.back().direction = static_cast<scrabble::Direction>(direction);
        legal_moves_.back().word = word;
        legal_moves_.back().score = -1;
    }

    const Mafsa*      mafsa_;
    std::vector<Move> legal_moves_;
};

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

std::optional<ReplayMove> parsemove_isc(const std::string& line)
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
    memset(&result.rack, 0, sizeof(result.rack));
    for (const char tile : rack) {
        cicero_rack_add_tile(&result.rack, tile);
    }
    return result;
}


bool replay_file(std::ifstream& ifs, const Mafsa& dict)
{
    FileType type = FileType::eInvalid;

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
            fmt::print(stderr, "error: game file doesn't begin with type identifier\n");
            return false;
        }
        break;
    }

    while (getline_stripped(ifs, line)) {
        if (line.empty()) {
            continue;
        }
        std::string key;
        std::string value;
        if (!re2::RE2::FullMatch(line, header_regex, &key, &value)) {
            fmt::print(stdout, "Found first non-header line: \"{}\"\n", line);
            break;
        }
        // fmt::print(stdout, "PGN Header: key=\"{}\" value=\"{}\"\n", key, value);
    }

    auto* parse_fn = &parsemove_isc;

    Callbacks cb(&dict);
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());

    do {
        if (line.empty() || line[0] == '#') {
            std::cout << "skipping line: \"" << line << "\"\n";
            continue;
        }
        std::cout << "line: \"" << line << "\"" << std::endl;
        auto maybe_replay_move = parsemove_isc(line);
        if (!maybe_replay_move) {
            continue;
        }
        auto& replay_move = *maybe_replay_move;
        using scrabble::operator<<;
        std::cout << "\t-> " << replay_move.move << " " << replay_move.rack << "\n";

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

        auto it = std::find_if(std::begin(legal_moves), std::end(legal_moves), match_ignoring_score);
        if (it == std::end(legal_moves)) {
            std::cerr << "error: did not find played move: " << replay_move.move << ", found moves " << legal_moves << "\n";
            return false;
        } else {
            std::cout << "generated move: " << replay_move.move << " correctly!\n";
        }

        // Desired API:
        //     if (cicero_legal_move(&engine, &move)) {
        //         int score = cicero_make_move(&engine, &move);
        //         // ... do something with it ...
        //         cicero_undo_move(&engine, &move);
        //     }

        auto engine_move = scrabble::EngineMove::make(&engine, replay_move.move);
        int fast_score = cicero_score_move_fast(&engine, &engine_move.move);
        int score = cicero_make_move(&engine, &engine_move.move);
        if (score != replay_move.move.score) {
            fmt::print(stderr, "Scores don't match :( => engine={} correct={}\n\n", score, replay_move.move.score);
            return false;
        } else {
            fmt::print(stdout, "Scores match! => engine={} correct={}\n\n", score, replay_move.move.score);
        }
        if (fast_score != replay_move.move.score) {
            fmt::print(stderr, "!!! FAIL !!! FAST score doesn't match :( => engine={} correct={}\n\n", fast_score, replay_move.move.score);
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
        fmt::print(stdout, "\n\nPassed {}!\n\n", filename);
    }

    return 0;
}
