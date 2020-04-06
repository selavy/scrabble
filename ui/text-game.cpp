#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <regex>
#include <editline/readline.h>
#include <editline/history.h>
#include "game.h"


const char* prompt = "\1\033[7m\1Edit$\1\033[0m\1 ";
static std::regex isc_regex(R"(((?:\d\d?[A-O])|(?:[A-O]\d\d?)) ([a-zA-Z]+) (\d+))", std::regex_constants::ECMAScript);

std::optional<IscMove> valid_isc_form(const char* isc) {
    std::cmatch match;
    if (std::regex_match(isc, match, isc_regex) && match.size() == 4) {
        IscMove move;
        move.sqspec = match[1];
        move.root   = match[2];
        auto score = match[3].str();
        move.score  = atoi(score.c_str());
        std::cout << "SQUARE SPEC: " << move.sqspec << "\n";
        std::cout << "ROOT WORD  : " << move.root   << "\n";
        std::cout << "SCORE      : " << move.score  << "\n";
        return move;
    }
    std::cerr << "Invalid input: '" << isc << "'" << std::endl;
    return std::nullopt;
}

std::optional<IscMove> get_move()
{
    char* buf;
retry:
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

    std::cout << "Quit? [Y/n]\n";
    while ((buf = readline(prompt)) != nullptr) {
        if (strlen(buf) == 0 || buf[0] == 'Y') {
            free(buf);
            return std::nullopt;
        }
        if (strlen(buf) > 0 && buf[0] == 'n') {
            free(buf);
            goto retry;
        }
        free(buf);
    }

    return std::nullopt;
}

int main(int argc, char **argv) {
    int seed = 42;
    auto board = std::make_unique<Board>();
    auto bag = std::make_unique<Bag>(seed);
    std::array<Rack, 2> racks;
    Player ptm = Player::Player1;

    { // TEMP TEMP
        DEBUG("--- DRAW ORDER ---");
        for (auto rit = bag->tiles.rbegin(); rit != bag->tiles.rend(); ++rit) {
            DEBUG("%c", *rit);
        }
        DEBUG("--- END DRAW ORDER ---");
    }

    for (;;) {
        std::cout << *board << std::endl;
        auto maybe_move = get_move();
        if (!maybe_move) {
            break;
        }
        auto move = *maybe_move;
        std::cout << "Got a move!\n\n";
    }

    return 0;
}
