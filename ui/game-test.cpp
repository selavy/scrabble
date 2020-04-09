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

EngineRack make_engine_rack(const std::string& s)
{
    EngineRack r;
    memset(&r, 0, sizeof(r));
    for (char c : s) {
        if (c == '?') {
            r.tiles[26]++;
        } else if (c == ' ') {
            continue;
        } else if ('A' <= c && c <= 'Z') {
            r.tiles[c - 'A']++;
        } else if ('a' <= c && c <= 'z') {
            r.tiles[c - 'a']++;
        } else {
            fmt::print(stderr, "invalid tile in rack specification: '{}' ({})\n", c, static_cast<int>(c));
            assert(0 && "invalid tile in rack specification");
        }
    }
    return r;
}

std::ostream& operator<<(std::ostream& os, const EngineRack& rack)
{
    auto to_ext = [](char tile) -> char {
        if (0 <= tile && tile < 26) {
            return tile + 'A';
        } else {
            assert(0 && "invalid rack tile");
            __builtin_unreachable();
            return 0;
        }
    };

    char buf[8];
    memset(buf, 0, sizeof(buf));
    std::size_t bidx = 0;
    for (int tile = 0; tile < 26; ++tile) {
        for (int i = 0; i < rack.tiles[tile]; ++i) {
            assert(bidx < 8);
            buf[bidx++] = to_ext(tile);
        }
    }
    for (int i = 0; i < rack.tiles[26]; ++i) {
        buf[bidx++] = '?';
    }
    buf[bidx] = 0;
    os << buf;
    return os;
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

struct LegalMoves
{
    std::vector<std::string> isc_specs;
    const EngineTrie*        trie;

    static void on_legal_move(void* data, const char* word, int lsq, int rsq, int dir) noexcept
    {
        auto* self = reinterpret_cast<LegalMoves*>(data);
        self->on_legal_move_(word, lsq, rsq, dir);
    }

    static std::string mk_isc_spec(std::string word, int sq, int dir)
    {
        std::string result;
        char row = (sq / 15) + 'A';
        int  col = (sq % 15) +  1;
        if (dir == 1) {
            result += row;
            result += std::to_string(col);
        } else if (dir == 15) {
            result += std::to_string(col);
            result += row;
        } else {
            assert(0 && "bad direction");
        }
        result += ' ';
        for (char c : word) {
            if ('a' <= c && c <= 'z') {
                result += 'A' + (c - 'a');
            } else if ('A' <= c && c <= 'Z') {
                result += 'a' + (c - 'A');
            } else {
                assert(0 && "invalid word");
            }
        }
        return result;
    }

    void on_legal_move_(const char* word, int lsq, int rsq, int dir)
    {
        bool terminal = false;;
        auto es = trie->children(word, terminal);
        assert(terminal == true);
        isc_specs.push_back(LegalMoves::mk_isc_spec(word, lsq, dir));
    }

    static Edges get_prefix_edges(void* data, const char* prefix) noexcept {
        auto* self = reinterpret_cast<LegalMoves*>(data);
        return self->get_prefix_edges_(prefix);
    }

    Edges get_prefix_edges_(const char* prefix)
    {
        Edges edges;
        auto es = trie->children(prefix, edges.terminal);
        std::size_t i = 0;
        for (auto ch : es) {
            edges.edges[i++] = ch;
        }
        edges.edges[i] = 0;
        return edges;
    }
};

IscMove _parse_isc_string(const std::string& s) {
    IscMove isc;
    char spec[32];
    char root[32];
    sscanf(s.c_str(), "%s %s %d", &spec[0], &root[0], &isc.score);
    isc.sqspec = spec;
    isc.root   = root;
    return isc;
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

    auto boardp = std::make_unique<Board>();
    auto enginep = std::make_unique<Engine>();
    auto& board = *boardp;
    auto* engine = enginep.get();
    LegalMoves lm;
    lm.trie = &dict;
    engine->on_legal_move = &LegalMoves::on_legal_move;
    engine->on_legal_move_data = &lm;
    engine->prefix_edges = &LegalMoves::get_prefix_edges;
    engine->prefix_edges_data = &lm;
    engine_init(engine);

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
        fmt::print(stdout, "[{}] {}\n", rack_spec, isc_spec);

        IscMove isc_move;
        isc_move.score = -1;
        re2::RE2::FullMatch(isc_spec, isc_regex, &isc_move.sqspec, &isc_move.root, &isc_move.score);
        if (isc_move.sqspec.empty() || isc_move.root.empty()) {
            fmt::print(stderr, "error: invalid ISC move: \"{}\"\n", isc_spec);
            return false;
        }

        EngineRack rack = make_engine_rack(rack_spec);

        // TEMP TEMP
        // std::cout << "Parsed ISC move: " << isc_move << " with rack = " << rack << "\n";

        lm.isc_specs.clear();
        engine_find(engine, rack);

        auto board_copy = std::make_unique<Board>(board);
        auto gui_move = make_gui_move_from_isc(board, isc_move);
        auto maybe_move = make_move(board, gui_move);
        assert(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        // std::cout << "\n" << move << std::endl;

        { // verify that all moves reported as legal are in fact legal
            // std::cout << "RACK: " << rack_spec << "\n";
            bool actual_move_in_legal_moves_list = false;
            for (auto isc_spec2 : lm.isc_specs) {
                auto isc_move2 = _parse_isc_string(isc_spec2);
                auto gui_move2 = make_gui_move_from_isc(*board_copy, isc_move2);
                // std::cout << "CHECKING: " << isc_spec2 << " " << gui_move2 << std::endl;
                auto maybe_move2 = make_move(*board_copy, gui_move2);
                assert(static_cast<bool>(maybe_move2) == true);
                undo_move(*board_copy, gui_move2);
                actual_move_in_legal_moves_list |= gui_move2 == gui_move;
            }
            // std::cout << "ACTUAL MOVE: " << isc_spec << " " << gui_move << std::endl;
            assert(actual_move_in_legal_moves_list == true);
        }

        EngineMove em;
        em.tiles   = &move.tiles[0];
        em.squares = &move.squares[0];
        em.ntiles  = static_cast<int>(gui_move.size());
        em.direction = static_cast<int>(move.direction);
        engine_make_move(engine, &em);

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
    if (replay_game(ifs, dict)) {
        fmt::print(stdout, "Passed!\n");
    } else {
        fmt::print(stderr, "error: unable to replay game file: \"{}\"\n", gamefile);
        return 1;
    }

    return 0;
}
