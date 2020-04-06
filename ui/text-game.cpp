#include <editline/history.h>
#include <editline/readline.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <regex>

#include <re2/re2.h>

#include "game.h"

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

int main(int argc, char** argv) {
    int seed = 42;
    auto board = std::make_unique<Board>();
    auto bag = std::make_unique<Bag>(seed);
    Player ptm = Player::Player1;
    std::array<Rack, 2> racks;
    std::fill(std::begin(racks[0]), std::end(racks[0]), Empty);
    std::fill(std::begin(racks[1]), std::end(racks[1]), Empty);

    {  // TEMP TEMP
        DEBUG("--- DRAW ORDER ---");
        for (auto rit = bag->tiles.rbegin(); rit != bag->tiles.rend(); ++rit) {
            DEBUG("%c", *rit);
        }
        DEBUG("--- END DRAW ORDER ---");
    }

    for (;;) {
        auto& rack = racks[static_cast<std::size_t>(ptm)];
        draw_tiles(*bag, rack);
        std::cout << *board << std::endl;
        auto maybe_isc_move = get_move(ptm, rack);
        if (!maybe_isc_move) {
            break;
        }
        auto isc_move = *maybe_isc_move;
        if (isc_move.sqspec.empty()) {
            std::cout << GetPlayerName(ptm) << " PASSED!\n";
        } else {
            std::cout << "DEBUG: Got a ISC move: " << isc_move << "\n";
            auto gui_move = make_gui_move_from_isc(*board, isc_move);
            std::cout << "DEBUG: Got a GUI move: " << gui_move << "\n";
            assert(no_squares_in_gui_move_are_not_empty(*board, gui_move));

            if (!rack_has_tiles(rack, gui_move)) {
                std::cerr << "Attempted to play tiles you don't have!" << std::endl;
                continue;
            }

            auto debug_board_copy = std::make_unique<Board>(*board);
            auto maybe_move = make_move(*board, gui_move);
            if (!maybe_move) {
                assert(*debug_board_copy == *board);
                // TODO: add error message about why?
                std::cerr << "Invalid move" << std::endl;
                continue;
            }
            auto move = *maybe_move;
            std::cout << "DEBUG: Got a move: " << move << "\n";
            assert(isc_move.score == -1 || move.score == isc_move.score);

            auto words = find_words(*board, move);
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
