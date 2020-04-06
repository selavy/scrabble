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

std::optional<IscMove> get_move() {
    bool quit = false;
    char* buf;
    do {
        while ((buf = readline(prompt)) != nullptr) {
            if (strlen(buf) > 0) {
                add_history(buf);
            }
            auto maybe_isc = valid_isc_form(buf);
            free(buf);
            if (maybe_isc) {
                return maybe_isc;
            }
        }

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

int main(int argc, char** argv) {
    int seed = 42;
    auto board = std::make_unique<Board>();
    auto bag = std::make_unique<Bag>(seed);
    std::array<Rack, 2> racks;
    Player ptm = Player::Player1;

    {  // TEMP TEMP
        DEBUG("--- DRAW ORDER ---");
        for (auto rit = bag->tiles.rbegin(); rit != bag->tiles.rend(); ++rit) {
            DEBUG("%c", *rit);
        }
        DEBUG("--- END DRAW ORDER ---");
    }

    for (;;) {
        std::cout << *board << std::endl;
        auto maybe_isc_move = get_move();
        if (!maybe_isc_move) {
            break;
        }
        auto isc_move = *maybe_isc_move;
        std::cout << "Got a ISC move: " << isc_move << std::endl;

        auto gui_move = make_gui_move_from_isc(*board, isc_move);
        std::cout << "Got a GUI move: " << gui_move << std::endl;
        assert(no_squares_in_gui_move_are_not_empty(*board, gui_move));

        auto debug_board_copy = std::make_unique<Board>(*board);
        auto maybe_move = make_move(*board, gui_move);
        if (!maybe_move) {
            assert(*debug_board_copy == *board);
            // TODO: add error message about why?
            std::cerr << "Invalid move" << std::endl;
            continue;
        }
        auto move = *maybe_move;
        std::cout << "Got a move: " << move << std::endl;
        assert(isc_move.score == -1 || move.score == isc_move.score);

        auto words = find_words(*board, move);
        std::cout << "Move Scored  : " << move.score << "\n";
        std::cout << "Created words: " << words << "\n";
    }

    std::cerr << "Bye." << std::endl;

    return 0;
}
