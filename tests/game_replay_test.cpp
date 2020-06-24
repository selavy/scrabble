#include <algorithm>
#include <fstream>
#include <optional>
#include <iostream>
#include <string>
#include <string_view>

// #include <fmt/format.h>
#include <fmt/ostream.h>
#include <cxxopts.hpp>
#include <re2/re2.h>

#include <mafsa++.h>
#include <scrabble.h>
#include <gcg.h>
#include "test_helpers.h"


re2::RE2 isc_regex(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)");
re2::RE2 move_line_regex(R"(\s*\"(.*)\", \"(.*)\"\s*)");
re2::RE2 header_regex(R"(\s*\[(\w+) \"(.*)\"]\s*)");
re2::RE2 empty_line_regex(R"(\s+)");
re2::RE2 change_line_regex(R"(\s*"?CHANGE\s+(\d+)\"?\s*)");


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

struct Logger
{
    enum class Level
    {
        eDebug = 0,
        eInfo  = 1,
        eWarn  = 2,
        eError = 3,
    };

    Logger(Level level) : level_{level} {}

    template <typename... Args>
    void log(Level level, const char* fmt, Args&&... args)
    {
        if (static_cast<int>(level) >= static_cast<int>(level_)) {
            fmt::print(std::cout, fmt, std::forward<Args>(args)...);
        }
    }

    Level level_;
};

#define DEBUG(fmt, ...) logger.log(Logger::Level::eDebug, "DEBUG: " fmt "\n", ##__VA_ARGS__)
#define INFO(fmt, ...)  logger.log(Logger::Level::eInfo,  "INFO: "  fmt "\n", ##__VA_ARGS__)
#define WARN(fmt, ...)  logger.log(Logger::Level::eWarn,  "WARN: "  fmt "\n", ##__VA_ARGS__)
#define ERROR(fmt, ...) logger.log(Logger::Level::eError, "ERROR: " fmt "\n", ##__VA_ARGS__)

bool apply_move(ReplayMove& replay_move, scrabble::EngineMove& engine_move,
        cicero& engine, cicero_savepos& sp, Callbacks& cb, Logger& logger)
{
    DEBUG("\tmove={} rack={}\n{}\n", replay_move.move, replay_move.rack, engine);

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
        auto it = std::find_if(std::begin(legal_moves), std::end(legal_moves),
                match_ignoring_score);
        if (it == std::end(legal_moves)) {
            ERROR("Did not find played move: {}, found moves {}",
                    replay_move.move, legal_moves);
            return false;
        } else {
            DEBUG("generated move: {} correctly", replay_move.move);
        }
    } else {
        WARN("move played that is not in dictionary: '{}'", replay_move.move.word);
    }

    // verify all generated moves are legal -- legality check doesn't use
    // cross-checks and such.
    for (const auto& move : legal_moves) {
        auto emove = scrabble::EngineMove::make(&engine, move);
        int rc = cicero_legal_move(&engine, &emove.move);
        if (rc != CICERO_LEGAL_MOVE) {
            ERROR("\n{}\n\nGenerated move {} not legal: {}.\nOn game move {}",
                    engine, move, cicero_legal_move_errnum_to_string(rc), replay_move.move);
            ERROR("\n\nLegal Moves:\n{}", legal_moves);

            auto I8 = hume::Square::from_isc_name("I8")->value();
            INFO("hchk[I8] = 0x{:08x} = {}", engine.hchk[I8], XChk{engine.hchk[I8]});
            INFO("vchk[I8] = 0x{:08x} = {}", engine.vchk[I8], XChk{engine.vchk[I8]});

            cicero engine2;
            cicero_init(&engine2, cb.make_callbacks());
            cicero_load_position_ex(&engine2, &engine);
            {
                for (int sq = 0; sq < 225; ++sq) {
                    assert(engine.vals[sq] == engine2.vals[sq]);
                }

                for (int sq = 0; sq < 225; ++sq) {
                    INFO(
                        "Checking: {}\n"
                        "\te1.hchk[{}] = 0x{:08x} = {}\n"
                        "\te2.hchk[{}] = 0x{:08x} = {}\n"
                        "\te1.vchk[{}] = 0x{:08x} = {}\n"
                        "\te2.vchk[{}] = 0x{:08x} = {}\n",
                        hume::Square::make(sq)->name(),
                        sq, engine.hchk[sq],  XChk{engine.hchk[sq]},
                        sq, engine2.hchk[sq], XChk{engine2.hchk[sq]},
                        sq, engine.vchk[sq],  XChk{engine.vchk[sq]},
                        sq, engine2.vchk[sq], XChk{engine2.vchk[sq]}
                    );
                    assert(engine.vals[sq] == engine2.vals[sq]);
                    // assert(engine.hscr[sq] == engine2.hscr[sq]);
                    // assert(engine.vscr[sq] == engine2.vscr[sq]);
                    assert(engine.hchk[sq] == engine2.hchk[sq]);
                    assert(engine.vchk[sq] == engine2.vchk[sq]);
                    // assert(isanchorsq(engine1, sq) == isanchorsq(engine2, sq));
                }
            }

            return false;
        }
    }

    // Desired API:
    //     if (cicero_legal_move(&engine, &move)) {
    //         int score = cicero_make_move(&engine, &move);
    //         // ... do something with it ...
    //         cicero_undo_move(&engine, &move);
    //     }

    engine_move = scrabble::EngineMove::make(&engine, replay_move.move);
    int rc = cicero_legal_move(&engine, &engine_move.move);
    if (rc != CICERO_LEGAL_MOVE) {
        ERROR("cicero_legal move returned: {}",
                cicero_legal_move_errnum_to_string(rc));
    }

    int score = cicero_make_move(&engine, &sp, &engine_move.move);
    if (score != replay_move.move.score) {
        ERROR("Scores don't match :( => engine={} correct={}",
                score, replay_move.move.score);
        return false;
    } else {
        DEBUG("Scores match! => engine={} correct={}",
                score, replay_move.move.score);
    }

    // TEMP -- check undo_move
    constexpr bool check_undo_move = false;
    if (check_undo_move) {
        cicero_undo_move(&engine, &sp, &engine_move.move);
        auto score2 = cicero_make_move(&engine, &sp, &engine_move.move);
        if (score != score2) {
            ERROR("after undo move scores don't match; old={} new={}",
                    score2, score);
            return false;
        }
    }

    return true;
}


ReplayMove make_replay_move(const hume::gcg::Play& move, cicero* engine)
{
    ReplayMove result;
    cicero_make_rack(&result.rack, move.rack.c_str());
    result.move.square = scrabble::Square(move.square.value());
    result.move.direction = move.direction == hume::Direction::Horz ? scrabble::Direction::Horz : scrabble::Direction::Vert;
    // result.move.word = move.word;
    result.move.word = "";
    int stride = static_cast<int>(result.move.direction);
    int start  = result.move.square.value();
    int step   = 0;
    for (char ch : move.word) {
        if (ch == '.') {
            ch = cicero_tile_on_square(engine, start + step * stride);
        }
        result.move.word += ch;
        ++step;
    }
    result.move.score = move.score;
    return result;
}

bool replay_file_gcg(std::ifstream& ifs, Callbacks& cb, cicero& engine,
        Logger& logger)
{
    std::string line;
    cicero_savepos sp;
    scrabble::EngineMove engine_move;
    hume::gcg::Parser parser;

    while (getline_stripped(ifs, line)) {
        auto move = parser.parse_line(line);
        if (auto* m = std::get_if<hume::gcg::Comment>(&move)) {
            DEBUG("skipping comment: \"{}\"", *m);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::Pragmata>(&move)) {
            DEBUG("skipping pragma: \"{}\"", line);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::Play>(&move)) {
            auto replay_move = make_replay_move(*m, &engine);
            if (!apply_move(replay_move, engine_move, engine, sp, cb, logger)) {
                return false;
            }
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::PassedTurn>(&move)) {
            DEBUG("skipping passed turn: \"{}\"", line);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::TileExchangeKnown>(&move)) {
            DEBUG("skipping tile exchange known: \"{}\"", line);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::TileExchangeCount>(&move)) {
            DEBUG("skipping tile exchange count: \"{}\"", line);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::PhoneyRemoved>(&move)) {
            DEBUG("withdrawing previous move");
            cicero_undo_move(&engine, &sp, &engine_move.move);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::ChallengeBonus>(&move)) {
            DEBUG("skipping challenge bonus: \"{}\"", line);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::LastRackPoints>(&move)) {
            DEBUG("skipping last rack points: \"{}\"", line);
            continue;
        }
        else if (auto* m = std::get_if<hume::gcg::TimePenalty>(&move)) {
            DEBUG("skipping time penalty: \"{}\"", line);
            continue;
        }
        else {
            ERROR("unknown line type: \"{}\"", line);
            throw std::runtime_error("Didn't handle all variant cases");
        }
    }

    return true;
}

bool replay_file(std::ifstream& ifs, Callbacks& cb, Logger& logger)
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
            WARN("game file doesn't have an identifier. assuming gcg.");
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
        }
    }

    cb.clear_legal_moves();
    cicero_savepos sp;
    cicero engine;
    cicero_init(&engine, cb.make_callbacks());
    scrabble::EngineMove engine_move;
    if (type == FileType::eIsc) {
        do {
            if (line.empty() || line[0] == '#') {
                INFO("skipping line \"{}\"", line);
                continue;
            }
            INFO("line: \"{}\"", line);
            auto maybe_replay_move = parsemove_isc(line, &engine);
            if (!maybe_replay_move) {
                continue;
            }
            auto& replay_move = *maybe_replay_move;
            if (!apply_move(replay_move, engine_move, engine, sp, cb, logger)) {
                return false;
            }
        } while (getline_stripped(ifs, line));
    } else {
        return replay_file_gcg(ifs, cb, engine, logger);
    }
    return true;
}

void mytest(Callbacks& cb)
{
    const std::vector<std::string> moves = {
        "H7 oaf",
    };

    fmt::print("--- MYTEST ---\n\n");

    cicero engine;
    cicero_savepos sp;
    char board[225];
    cicero_init(&engine, cb.make_callbacks());

    for (const auto& isc_move : moves) {
        auto smove = scrabble::Move::from_isc_spec(isc_move.c_str());
        auto emove = scrabble::EngineMove::make(&engine, smove);
        auto* move = &emove.move;
        cicero_make_move(&engine, &sp, move);
        std::cout << engine << "\n\n";
        auto I8 = hume::Square::from_isc_name("I8")->value();
        fmt::print("hchk[I8] = 0x{:08x} = {}\n", engine.hchk[I8], XChk{engine.hchk[I8]});
        fmt::print("vchk[I8] = 0x{:08x} = {}\n", engine.vchk[I8], XChk{engine.vchk[I8]});
    }

    fmt::print("--- END MYTEST ---\n\n");
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
        ("d,dict", "dictionary file to use", cxxopts::value<std::string>()->default_value("csw19.dict.gz"), "DICT")
        ("f,file", "input game file", cxxopts::value<std::vector<std::string>>())
        ("v,verbose", "verbose output", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "print usage")
        ;
    options.positional_help("[FILE...]");
    options.parse_positional({"file"});
    auto args = options.parse(argc, argv);
    if (args.count("help")) {
        std::cerr << options.help() << std::endl;
        return 0;
    }
    // clang-format on

    if (!args["file"].count()) {
        std::cerr << "Must specify at least one file to replay\n\n"
            << options.help() << std::endl;
        return 0;
    }

    auto gamefiles = args["file"].as<std::vector<std::string>>();
    auto dictfile  = args["dict"].as<std::string>();
    auto maybe_dict = Mafsa::load(dictfile);
    if (!maybe_dict) {
        std::cerr << "unable to load dictionary from file: " << dictfile << std::endl;
        return 1;
    }
    auto& dict = *maybe_dict;
    auto cb = Callbacks{std::move(dict)};
    auto logger = Logger{args["verbose"].as<bool>() ? Logger::Level::eDebug : Logger::Level::eInfo};

    mytest(cb);

    for (const auto& filename : gamefiles) {
        INFO("Replaying {}", filename);
        std::ifstream ifs{filename};
        if (!ifs) {
            ERROR("unable to open game file: {}", filename);
            return 1;
        }
        try {
            if (!replay_file(ifs, cb, logger)) {
                ERROR("failed replaying game file: {}", filename);
                return 1;
            }
        } catch (...) {
            ERROR("Failed in: {}", filename);
            throw;
        }
        INFO("\nPassed {}!\n", filename);
    }

    return 0;
}
