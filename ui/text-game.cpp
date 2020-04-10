#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <regex>
#include <fmt/format.h>
#include <cxxopts.hpp>
#include <re2/re2.h>
#include <editline/history.h>
#include <editline/readline.h>

#include "game.h"
#include "engine.h"

const char* prompt = "\1\033[7m\1Scrabble$\1\033[0m\1 ";

#ifdef USE_STD_REGEX
static std::regex isc_regex(R"(\s*((?:\d\d?[A-O])|(?:[A-O]\d\d?))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)", std::regex_constants::ECMAScript);
#else
static re2::RE2 isc_regex(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]+)(?:\s+(\d+))?\s*)");
#endif

std::optional<IscMove> valid_isc_form(const char* isc) {
#if USE_STD_REGEX
    std::cmatch match;
    if (std::regex_match(isc, match, isc_regex) && match.size() == 4) {
        // NOTE(peter): strangely (to me) the regex library reports 4 matches
        // even in the case that it doesn't match with the final \d+ group so
        // have to test if the match string is empty instead.
        IscMove move;
        move.sqspec = match[1];
        move.root   = match[2];
        move.score  = -1;
        if (match[3].length() != 0) {
            auto score = match[3].str();
            move.score = atoi(score.c_str());
        }
#else
    assert(isc_regex.ok());
    IscMove move;
    move.score = -1;
    re2::RE2::FullMatch(isc, isc_regex, &move.sqspec, &move.root, &move.score);
    if (!move.sqspec.empty() && !move.root.empty()) {
#endif
        std::cout << "SQUARE SPEC: " << move.sqspec << "\n";
        std::cout << "ROOT WORD  : " << move.root << "\n";
        std::cout << "SCORE      : " << move.score << "\n";
        return move;
    }
    std::cerr << "Invalid input: '" << isc << "'" << std::endl;
    return std::nullopt;
}

std::optional<IscMove> get_move(Player player, const Rack& rack) {
    bool quit = false;
    char* buf;
    do {
        std::cout << GetPlayerName(player) << " rack: " << rack << "\n";
        while ((buf = readline(prompt)) != nullptr) {
            if (strlen(buf) > 0) {
                add_history(buf);
            }
            if (strcasecmp(buf, "pass") == 0) {
                free(buf);
                return IscMove{};
            }
            auto maybe_isc = valid_isc_form(buf);
            free(buf);
            if (maybe_isc) {
                return maybe_isc;
            }
        }

        std::cout << "\n";
        bool keep_asking = true;
        do {
            buf = readline("Quit? [Y/n] ");
            if (buf == nullptr) {
                quit = true;
                keep_asking = false;
            } else if (strlen(buf) == 0) {
                quit = true;
                keep_asking = false;
            } else if (buf[0] == 'Y' || buf[0] == 'y') {
                quit = true;
                keep_asking = false;
            } else if (strncasecmp(buf, "no", 2) == 0) {
                quit = false;
                keep_asking = false;
            } else {
                printf("Unknown input: '%s'\n", buf);
            }
            free(buf);
        } while (keep_asking);

    } while (!quit);
    return std::nullopt;
}

bool no_squares_in_gui_move_are_not_empty(const Board& board, const GuiMove& gui_move) {
    for (auto&& [tile, square] : gui_move) {
        if (board.brd[square] != Empty) {
            return false;
        }
    }
    return true;
}

void remove_tiles_from_rack(Rack& rack, const GuiMove& move) {
    for (auto [tile, square] : move) {
        if (isBlankTile(tile)) {
            tile = Empty;
        }
        auto it = std::find(std::begin(rack), std::end(rack), tile);
        assert(it != std::end(rack));
        *it = Empty;
    }
}

struct LegalMoves {
    std::vector<std::string> isc_specs;
    const EngineTrie* trie;

    static void on_legal_move(void* data, const char* word, int lsq, int rsq, int dir) noexcept {
        auto* self = reinterpret_cast<LegalMoves*>(data);
        self->on_legal_move_(word, lsq, rsq, dir);
    }

    static std::string mk_isc_spec(std::string word, int sq, int dir) {
        std::string result;
        char row = (sq / 15) + 'A';
        int col = (sq % 15) + 1;
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

    void on_legal_move_(const char* word, int lsq, int rsq, int dir) {
        bool terminal = false;
        ;
        auto es = trie->children(word, terminal);
        assert(terminal == true);
        isc_specs.push_back(LegalMoves::mk_isc_spec(word, lsq, dir));
    }

    static Edges get_prefix_edges(void* data, const char* prefix) noexcept {
        auto* self = reinterpret_cast<LegalMoves*>(data);
        return self->get_prefix_edges_(prefix);
    }

    Edges get_prefix_edges_(const char* prefix) {
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

std::optional<EngineTrie> load_dictionary(std::string path) {
    EngineTrie dict;
    std::string word;
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

int main(int argc, char** argv) {
    // clang-format off
    cxxopts::Options options("text-game", "play against engine");
    options.add_options()
        ("d,dict", "dictionary file to use", cxxopts::value<std::string>(), "DICT")
        ;
    options.parse_positional({"dict", "too many arguments" });
    auto args = options.parse(argc, argv);
    // clang-format on

    if (args.count("help")) {
        std::cerr << options.help() << std::endl;
        return 0;
    }
    if (!args.count("dict")) {
        fmt::print(stderr, "error: must specify dictionary file to use.\n");
        return 1;
    }

    auto dictfile  = args["dict"].as<std::string>();
    fmt::print(stdout, "Loading dictionary....\n");
    auto maybe_dict = load_dictionary(dictfile);
    if (!maybe_dict) {
        fmt::print(stderr, "error: unable to load dictionary from file: \"{}\"\n", dictfile);
        return 1;
    }
    const auto& dict = *maybe_dict;

    auto enginep = std::make_unique<Engine>();
    auto boardp = std::make_unique<Board>();
    auto bagp = std::make_unique<Bag>(/*seed*/42);

    auto* engine = enginep.get();
    auto& board = *boardp;
    auto& bag = *bagp;
    Player ptm = Player::Player1;
    std::array<Rack, 2> racks;
    std::fill(std::begin(racks[0]), std::end(racks[0]), Empty);
    std::fill(std::begin(racks[1]), std::end(racks[1]), Empty);

    LegalMoves lm;
    lm.trie = &dict;
    engine->on_legal_move = &LegalMoves::on_legal_move;
    engine->on_legal_move_data = &lm;
    engine->prefix_edges = &LegalMoves::get_prefix_edges;
    engine->prefix_edges_data = &lm;
    engine_init(engine);

    {  // TEMP TEMP
        DEBUG("--- DRAW ORDER ---");
        for (auto rit = bag.tiles.rbegin(); rit != bag.tiles.rend(); ++rit) {
            DEBUG("%c", *rit);
        }
        DEBUG("--- END DRAW ORDER ---");
    }

    for (;;) {
        auto& rack = racks[static_cast<std::size_t>(ptm)];
        draw_tiles(bag, rack);
        std::cout << board << std::endl;
        auto maybe_isc_move = get_move(ptm, rack);
        if (!maybe_isc_move) {
            break;
        }
        auto isc_move = *maybe_isc_move;
        if (isc_move.sqspec.empty()) {
            std::cout << GetPlayerName(ptm) << " PASSED!\n";
        } else {
            std::cout << "DEBUG: Got a ISC move: " << isc_move << "\n";
            auto gui_move = make_gui_move_from_isc(board, isc_move);
            std::cout << "DEBUG: Got a GUI move: " << gui_move << "\n";
            assert(no_squares_in_gui_move_are_not_empty(board, gui_move));

            if (!rack_has_tiles(rack, gui_move)) {
                std::cerr << "Attempted to play tiles you don't have!" << std::endl;
                continue;
            }

            auto maybe_move = make_move(board, gui_move);
            if (!maybe_move) {
                // TODO: add error message about why?
                fmt::print(stderr, "error: invalid move\n");
                continue;
            }
            auto move = *maybe_move;
            std::cout << "DEBUG: Got a move: " << move << "\n";
            assert(isc_move.score == -1 || move.score == isc_move.score);

            auto words = find_words(board, move);
            std::cout << "--------------------------------------\n";
            std::cout << "| Move Scored  : " << move.score  << "\n";
            std::cout << "| Created words: " << words       << "\n";
            std::cout << "--------------------------------------\n";
            // TODO: include tile frequencies in the rack instead so can do
            //       tile check and removal faster
            remove_tiles_from_rack(rack, gui_move);
        }

        ptm = flip_player(ptm);
    }

    std::cerr << "Bye." << std::endl;

    return 0;
}
