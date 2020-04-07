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

EngineRack make_engine_rack(const std::string& s)
{
    EngineRack r;
    memset(&r, 0, sizeof(r));
    for (char c : s) {
        if (c == '?') {
            r.tiles[26]++;
        } else {
            assert('A' <= c && c <= 'Z');
            r.tiles[c - 'A']++;
        }
    }
    return r;
}

void crosscheck_tests()
{
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
    // NOTE: the main word will NOT be checked so it doesn't need to be in the
    // dictionary for this check
    std::vector<std::string> should_fail_moves = {
        "K5 it",     // no "TT" formed vertically
        "5M let",    // no "LL" formed horizontally
        "9F rag",    // no "RA" formed horizontally
        "11F man",   // no "ZAGSM" formed horizontally
        "O6 OSSIFY", // no "SO" formed vertically
    };

    engine_init(engine, /*check_word*/&is_word, &dict, /*legal_move*/NULL, NULL);
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
            return;
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
            return;
        }

        undo_move(board, gui_move);
        printf("\n\n"); // TEMP TEMP
    }
#endif

    printf("\nPASSED!\n");
}

void on_legal_move(void* data, const char* word, int sq, int dir) {
    printf("FOUND LEGAL MOVE: %s at %s (%d) dir=%d\n", word, SQ_(sq), sq, dir);
}

void find_tests()
{
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

    std::vector<std::pair<std::string, std::string>> isc_moves = {
        // move           , rack before move
        { "H7  zag     26", "DGZTAAT" },
        { "I6  bam     24", "IMETRAB" },
        // { "J5  tag     25", "GTAADTS" },
        // { "8F  tram     6", "TTRRENI" },
        // { "K5  od      16", "T?SDAOD" },
        // { "L4  arenite 76", "EAITNER" },
        // { "10B pEdants 81", "SPDNA?T" },
    };

    engine_init(engine, &is_word, &dict, &on_legal_move, NULL);
    for (auto [isc_spec, rack_spec] : isc_moves) {
        const auto rack = make_engine_rack(rack_spec);
        DEBUG("making move: '%s'", isc_spec.c_str());
        // engine_print_anchors(engine);
        std::cout << board << std::endl;
        engine_print_anchors(engine);
        engine_find(engine, rack);

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

        engine_make_move(engine, &em);
        printf("\n\n"); // TEMP TEMP
    }

    // engine_print_anchors(engine);
    std::cout << board << std::endl;
}

int main(int argc, char** argv) {
    // crosscheck_tests();
    find_tests();
    return 0;
}
