#include <iostream>
#include <cassert>
#include "game.h"
#include "engine.h"
#include <unordered_set>

using Dict = std::unordered_set<std::string>;

void trie_tests()
{
    std::vector<std::string> words = {
        "AM",
        "BA",
        "BAM",
        "IT",
        "SAG",
        "SILLY",
        "STAG",
        "STAGS",
        "TAG",
        "TAGS",
        "TRAM",
        "TRAMS",
        "TRAIN",
        "TRAINS",
        "TRAP",
        "TRR",
        "ZA",
        "ZAG",
        "ZAGS",
    };
    EngineTrie trie;
    for (const auto& word : words) {
        trie.insert(word.c_str());
    }
    trie.print(std::cout);

    for (const auto& word : words) {
        assert(trie.is_word(word));
    }

    std::vector<std::string> not_words = {
        "AMZ",
        "BAZZ",
        "ZBAM",
        "ITZ",
        "SAGX",
        "SILLYA",
        "STA",
        "ZAGSX",
    };

    for (const auto& word : not_words) {
        assert(!trie.is_word(word));
    }

    { // works from a prefix
        auto result = trie.children("TRA");
        std::cout << "Children of prefix \"TRA\"\n";
        for (auto c : result) {
            std::cout << c << " ";
        }
        std::cout << "\n";
        assert(std::find(result.begin(), result.end(), 'I') != result.end());
        assert(std::find(result.begin(), result.end(), 'M') != result.end());
        assert(std::find(result.begin(), result.end(), 'P') != result.end());
    }

    { // works at root
        auto result = trie.children("");
        std::cout << "Children of prefix \"\"\n";
        for (auto c : result) {
            std::cout << c << " ";
        }
        std::cout << "\n";
        assert(std::find(result.begin(), result.end(), 'A') != result.end());
        assert(std::find(result.begin(), result.end(), 'B') != result.end());
        assert(std::find(result.begin(), result.end(), 'I') != result.end());
        assert(std::find(result.begin(), result.end(), 'S') != result.end());
        assert(std::find(result.begin(), result.end(), 'T') != result.end());
        assert(std::find(result.begin(), result.end(), 'Z') != result.end());
    }
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
        } else if (c == ' ') {
            continue;
        } else {
            assert('A' <= c && c <= 'Z');
            r.tiles[c - 'A']++;
        }
    }
    return r;
}

const char *GetDirName(int dir)
{
    if (dir == 1) {
        return "HORZ";
    } else if (dir == 15) {
        return "VERT";
    } else {
        return "UNKN";
    }
}

struct LegalMoves
{
    LegalMoves(Dict dict)
    {
        for (const auto& word : dict) {
            trie.insert(word);
        }
    }

    void reset() {
        isc_specs.clear();
    }

    std::vector<std::string> isc_specs;
    EngineTrie               trie;
};

std::string mk_isc_spec(std::string word, int sq, int dir)
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

void on_legal_move(void* data, const char* word, int lsq, int rsq, int dir) {
    auto lm = reinterpret_cast<LegalMoves*>(data);
    auto& trie = lm->trie;
    bool terminal = false;;
    auto es = trie.children(word, terminal);
    assert(terminal == true);
    lm->isc_specs.push_back(mk_isc_spec(word, lsq, dir));
    printf("FOUND LEGAL MOVE: %s at [%s, %s ] dir=%s => isc=%s\n",
            word, SQ_(lsq), SQ_(rsq), GetDirName(dir), lm->isc_specs.back().c_str());
}

Edges get_prefix_edges(void* data, const char* prefix) {
    Edges edges;
    auto* trie = reinterpret_cast<EngineTrie*>(data);
    auto es = trie->children(prefix, edges.terminal);
    std::size_t i = 0;
    for (auto ch : es) {
        edges.edges[i++] = ch;
    }
    edges.edges[i] = 0;
    // printf(" -- get_prefix_edges(\"%s\") = %s term=%s\n", prefix, edges.edges, edges.terminal?"TRUE":"FALSE");
    return edges;
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

    LegalMoves legal_moves{dict};
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

    engine->on_legal_move = &on_legal_move;
    engine->on_legal_move_data = &legal_moves;
    engine->prefix_edges = &get_prefix_edges;
    engine->prefix_edges_data = &legal_moves.trie;
    engine_init(engine);

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

void find_tests()
{
    Dict dict = {
        "AM",
        "ARENITE",
        "AT",
        "AVOW",
        "AVOWS",
        "AX",
        "AYE",
        "BA",
        "BAD",
        "BADE",
        "BAM",
        "CHIP",
        "CHIPS",
        "ER",
        "EYE",
        "FE",
        "GAFFER",
        "GI",
        "IT",
        "JIB",
        "JIBE",
        "JIBES",
        "KINDLER",
        "MINI",
        "OD",
        "ODA",
        "OH",
        "OHS",
        "OVEN",
        "OW",
        "OY",
        "NOD",
        "NU",
        "QAT",
        "QI",
        "PEDANTS",
        "RE",
        "REQUITE",
        "SAG",
        "SILLY",
        "SIRE",
        "STAG",
        "STAGS",
        "TAGS",
        "TAG",
        "TO",
        "TOECLIP",
        "TOR",
        "TRAM",
        "TRAMS",
        "US",
        "VAIN",
        "VOW",
        "WELLIE",
        "WOKE",
        "XU",
        "YO",
        "YE",
        "ZA",
        "ZAG",
        "ZAGS",

        // fake words to test bounds checking
        "LOVEN",
        "GLOVEN",
    };

    LegalMoves legal_moves{dict};
    auto boardp = std::make_unique<Board>();
    auto enginep = std::make_unique<Engine>();
    auto& board = *boardp;
    auto* engine = enginep.get();

    std::vector<std::pair<std::string, std::string>> isc_moves = {
        // move           , rack before move
        { "H7  zag     26", "DGZTAAT" },
        { "I6  bam     24", "IMETRAB" },
        { "J5  tag     25", "GTAADTS" },
        { "8F  tram     6", "TTRRENI" },
        { "K5  od      16", "T?SDAOD" },
        { "L4  arenite 76", "EAITNER" },
        { "10B pEdants 81", "SPDNA?T" },
        { "9C  yo      20", "POOTLYI" },
        { "8K  jib     12", "LBSCJHA" },
        { "N6  toeclip 78", "IECOLTP" },
        { "O6  ohs     67", "SHCAOVL" },
        { "F6  qat     32", "AIEMNQL" },
        { "12K chip    22", "CHVIALG" },
        { "G3  mini    20", "LLEIINM" },
        { "8A  oven    34", "VNAOLGE" },
        { "4C  wellie  20", "LLWEERE" },
        { "O12 sire    28", "IRI?RSE" },
        { "14J gaffer  34", "GEFNAFL" },
        { "15F reQuite 32", "EEU?RIT" },
        // new ---
        { "14A kindler 84", "ILEKNRD" },
        { "A12 woke    45", "YEORDWO" },
        { "11E xu      38", "NXSUVAI" },
        { "3C  oy      25", "DOOYRUA" },
        { "2C  vain    24", "SA VNUI" },
        { "1A  oda     22", "RD A UO" },
        { "5B  us      13", "  US   " },
    };

    engine->on_legal_move = &on_legal_move;
    engine->on_legal_move_data = &legal_moves;
    engine->prefix_edges = &get_prefix_edges;
    engine->prefix_edges_data = &legal_moves.trie;
    engine_init(engine);

    for (auto [isc_spec, rack_spec] : isc_moves) {
        const auto rack = make_engine_rack(rack_spec);
        engine_print_anchors(engine);

        // ACTUAL MOVE: O12 sire    28 [ (S, O12) (I, O13) (R, O14) (E, O15) ]
        printf("\n\nXChecks:\n");
        const int SQ_O12 = SQIX('O', 12);
        const int SQ_O13 = SQIX('O', 13);
        const int SQ_O14 = SQIX('O', 14);
        const int SQ_O15 = SQIX('O', 15);
        printf("HORZ:\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%s\n",
                GetSqName(SQ_O12), MBUF(engine->hchk[SQ_O12]),
                GetSqName(SQ_O13), MBUF(engine->hchk[SQ_O13]),
                GetSqName(SQ_O14), MBUF(engine->hchk[SQ_O14]),
                GetSqName(SQ_O15), MBUF(engine->hchk[SQ_O15])
        );
        printf("VERT:\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%s\n",
                GetSqName(SQ_O12), MBUF(engine->vchk[SQ_O12]),
                GetSqName(SQ_O13), MBUF(engine->vchk[SQ_O13]),
                GetSqName(SQ_O14), MBUF(engine->vchk[SQ_O14]),
                GetSqName(SQ_O15), MBUF(engine->vchk[SQ_O15])
        );
        printf("\n\n");
        // std::cout << board << std::endl;
        legal_moves.reset();
        engine_find(engine, rack);

        auto board_copy = std::make_unique<Board>(board);
        auto isc_move = _parse_isc_string(isc_spec);
        auto gui_move = make_gui_move_from_isc(board, isc_move);
        auto maybe_move = make_move(board, gui_move);
        assert(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        std::cout << "\n" << move << std::endl;

        { // verify that all moves reported as legal are in fact legal
            std::cout << "RACK: " << rack_spec << "\n";
            bool actual_move_in_legal_moves_list = false;
            for (auto isc_spec2 : legal_moves.isc_specs) {
                auto isc_move2 = _parse_isc_string(isc_spec2);
                auto gui_move2 = make_gui_move_from_isc(*board_copy, isc_move2);
                std::cout << "CHECKING: " << isc_spec2 << " " << gui_move2 << std::endl;
                auto maybe_move2 = make_move(*board_copy, gui_move2);
                assert(static_cast<bool>(maybe_move2) == true);
                undo_move(*board_copy, gui_move2);
                actual_move_in_legal_moves_list |= gui_move2 == gui_move;
            }
            std::cout << "ACTUAL MOVE: " << isc_spec << " " << gui_move << std::endl;
            assert(actual_move_in_legal_moves_list == true);
        }

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

    printf("\n\nPassed!\n");
}

int main(int argc, char** argv) {
    // std::cout << mk_isc_spec("HELLO", 0, 1 ) << std::endl;
    // std::cout << mk_isc_spec("HeLLO", 0, 15) << std::endl;
    trie_tests();
    crosscheck_tests();
    find_tests();
    return 0;
}
