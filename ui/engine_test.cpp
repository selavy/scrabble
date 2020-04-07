#include <iostream>
#include <cassert>
#include "game.h"
#include "engine.h"
#include <unordered_set>

using Dict = std::unordered_set<std::string>;

int is_word(void* data, const char* word) {
    auto& dict = *reinterpret_cast<Dict*>(data);
    return dict.count(word);
}

IscMove _parse_isc_string(std::string s) {
    IscMove isc;
    char spec[32];
    char root[32];
    sscanf(s.c_str(), "%s %s %d", &spec[0], &root[0], &isc.score);
    isc.sqspec = spec;
    isc.root   = root;
    return isc;
}

int main(int argc, char** argv) {
    Dict dict = {
        "AM",
        "BAM",
        "ZA",
        "ZAG",
        "TAG",
        "BA",
        "TRAM",
        "ZAGS",
        "SAG",
        "TAGS",
        "TRAMS",
        "STAG",
        "STAGS",
        "SILLY",
        "IT",
    };
    auto boardp = std::make_unique<Board>();
    auto enginep = std::make_unique<Engine>();
    auto& board = *boardp;
    auto* engine = enginep.get();

    std::vector<std::string> isc_moves = {
        "H7 zag 26",
        "I6 bam 24",
        "J5 tag 25",
        "8F tram 6",
        "10H sag 14",
        "F8 trams 9",
        "4J silly 21",
        "N4 yes 8",
    };
    // played after all `isc_moves` have been played on board
    std::vector<std::string> should_fail_moves = {
        "K5 it", // no "TT" formed vertically
    };

    engine_init(engine, &is_word, &dict);
    for (auto isc_spec : isc_moves) {
        DEBUG("making move: '%s'", isc_spec.c_str());
        engine_print_anchors(engine);
        std::cout << board << std::endl;
        auto isc_move = _parse_isc_string(isc_spec);
        auto gui_move = make_gui_move_from_isc(board, isc_move);
        auto maybe_move = make_move(board, gui_move);
        assert(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        std::cout << move << std::endl;

        EngineMove em;
        em.tiles   = &move.tiles[0];
        em.squares = &move.squares[0];
        em.ntiles  = static_cast<int>(gui_move.size());
        em.direction = static_cast<int>(move.direction);

        int sq = engine_xchk(engine, &em);
        if (sq != 0) {
            printf("FAILED: result of engine_xch: %s %d\n", SQ_(sq), sq);
            return 1;
        } else {
            printf("passed xchk! (%d)\n", sq);
        }
        engine_make_move(engine, &em);

        printf("\n\n"); // TEMP TEMP
    }

    engine_print_anchors(engine);
    std::cout << board << std::endl;

#if 1
    for (auto isc_spec : should_fail_moves) {
        DEBUG("making move: '%s'", isc_spec.c_str());
        std::cout << board << std::endl;
        auto isc_move = _parse_isc_string(isc_spec);
        auto gui_move = make_gui_move_from_isc(board, isc_move);
        auto maybe_move = make_move(board, gui_move);
        assert(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        std::cout << move << std::endl;

        EngineMove em;
        em.tiles   = &move.tiles[0];
        em.squares = &move.squares[0];
        em.ntiles  = static_cast<int>(gui_move.size());
        em.direction = static_cast<int>(move.direction);

        int sq = engine_xchk(engine, &em);
        if (sq != 0) {
            printf("PASSED: engine_xch: %s %d (expected xc failure)\n", SQ_(sq), sq);
        } else {
            printf("FAILED: passed xchk when not expected! (%d)\n", sq);
            return 1;
        }

        undo_move(board, gui_move);
        printf("\n\n"); // TEMP TEMP
    }
#endif

    printf("\nPASSED!\n");

    return 0;
}
