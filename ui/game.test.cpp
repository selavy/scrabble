#include <catch2/catch.hpp>
#include "game.h"
#include <iostream>

#define SQR(x) AsSquare(x)

TEST_CASE("Can insert words into set")
{
    for (std::size_t i = 0; i < all_empty_tiles.size(); ++i) {
        REQUIRE(all_empty_tiles[i] == Empty);
    }

    Dictionary dict;
    auto word1 = Word{"HELLO"};
    auto word2 = Word{"HELL"};
    auto word3 = Word{"HELLO"};
    auto word4 = Word{"MISSING"};
    REQUIRE(word1 != word2);
    REQUIRE(word1 == word1);
    REQUIRE(word1 == word3);
    REQUIRE(word1 != word4);
    REQUIRE(word2 != word4);

    dict.insert(word1);
    dict.insert(word2);
    REQUIRE(dict.size() == 2u);
    dict.insert(word3);
    REQUIRE(dict.size() == 2u);

    SECTION("Find present word \'HELLO\'")
    {
        auto iter = dict.find(word1);
        REQUIRE(iter != dict.end());
        REQUIRE(*iter == word1);
    }

    SECTION("Find present word \'HELL\'")
    {
        auto word = word2;
        auto iter = dict.find(word);
        REQUIRE(iter != dict.end());
        REQUIRE(*iter == word);
    }

    SECTION("Don\'t find present word \'HELL\'")
    {
        REQUIRE(dict.find(word4) == dict.end());
    }
}

bool tiles_have_word(const Tiles& tile, std::string word)
{
    assert(word.size() <= tile.size());
    size_t i = 0;
    for (i = 0; i < word.size(); ++i) {
        if (word[i] != tile[i]) {
            return false;
        }
    }
    for (; i < tile.size(); ++i) {
        if (tile[i] != Empty) {
            return false;
        }
    }
    return true;
}

TEST_CASE("Scrabble rules example", "[move_validation]")
{
	auto board = std::make_unique<Board>();

    std::vector<std::tuple<GuiMove, Player, Score, Sq, Direction, int, std::string, std::string>> ts = {
        {
            {
                { 'H', SQR(Sq::H6) },
                { 'O', SQR(Sq::H7) },
                { 'R', SQR(Sq::H8) },
                { 'N', SQR(Sq::H9) },
            },
            Player::Player1, 14, Sq::H6, Direction::HORIZONTAL, 4, "HORN", "HORN"
        },
        {
            {
                { 'F', SQR(Sq::F8) },
                { 'A', SQR(Sq::G8) },
                // { 'R', SQR(Sq::H8) },
                { 'M', SQR(Sq::I8) },
            },
            Player::Player2, 9, Sq::F8, Direction::VERTICAL, 4, "FAM", "FARM"
        },
        {
            {
                { 'P', SQR(Sq::J6 ) },
                { 'A', SQR(Sq::J7 ) },
                { 'S', SQR(Sq::J8 ) },
                { 'T', SQR(Sq::J9 ) },
                { 'E', SQR(Sq::J10) },
            },
            Player::Player1, 25, Sq::J6, Direction::HORIZONTAL, 5, "PASTE", "PASTE"
        },
        {
            {
                // { 'M', SQR(Sq::I8 ) },
                { 'O', SQR(Sq::I9 ) },
                { 'B', SQR(Sq::I10) },
            },
            Player::Player2, 16, Sq::I8, Direction::HORIZONTAL, 3, "OB", "MOB"
        },
        {
            {
                { 'B', SQR(Sq::K5 ) },
                { 'I', SQR(Sq::K6 ) },
                { 'T', SQR(Sq::K7 ) },
            },
            Player::Player1, 16, Sq::K5, Direction::HORIZONTAL, 3, "BIT", "BIT"
        },
    };

    for (auto&& [gmove, player, score, square, direction, length, tiles, root_word] : ts) {
        // std::cerr << "BEFORE:\n" << *board << std::endl;
        INFO("Placing root word: " << root_word);
        auto maybe_move = make_move(*board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == AsSquare(square));
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        CHECK(tiles_have_word(move.tiles, tiles));
        CHECK(move.root_word == root_word);
        // std::cerr << "AFTER:\n" << *board << "\n" << std::endl;
    }
}

// clang-format off
struct GuiTestCase
{
    GuiTestCase()=default;
    GuiTestCase(GuiMove gm, Player p, Score s, Sq sq, Direction d, int l)
        : gmove{gm}, player{p}, score{s}, square{AsSquare(sq)}, direction{d}, length{l} {}

    GuiMove   gmove;
    Player    player;
    Score     score;
    Square    square;
    Direction direction;
    int       length;
};
// clang-format on

GuiTestCase make_test_case_from_isc(const Board& b, IscMove isc)
{
    GuiTestCase t;
    auto& board = b.brd;
    auto&& [row, col, direction] = _parse_isc_spec(isc.sqspec);
    auto root = convert_to_internal_word(isc.root);
    t.player = b.n_moves % 2 == 0 ? Player::Player1 : Player::Player2;
    t.score = isc.score;
    t.square = row*Dim + col;
    t.direction = direction;
    t.length = static_cast<int>(root.size());
    const int start = t.square;
    const int step = static_cast<int>(t.direction);
    const int stop = start + step*Dim;
    assert (start + step * t.length < stop);
    for (size_t i = 0; i < root.size(); ++i) {
        const int sq = start + i * step;
        if (board[sq] == Empty) {
            t.gmove.emplace_back(root[i], sq);
        } else {
            assert(board[sq] == root[i]);
        }
    }
    return t;
}

TEST_CASE("selavy v andybfan example")
{
    auto board = std::make_unique<Board>();

    std::vector<GuiTestCase> moves =
    {
        {
            {
                { 'S', SQR(Sq::H8 ) },
                { 'E', SQR(Sq::H9 ) },
                { 'E', SQR(Sq::H10) },
            },
            Player::Player1, 6, Sq::H8, Direction::HORIZONTAL, 3
        },
        {
            {
                { 'M', SQR(Sq::H11) },
                { 'A', SQR(Sq::I11) },
                { 'Y', SQR(Sq::J11) },
                { 'O', SQR(Sq::K11) },
            },
            Player::Player2, 24, Sq::H11, Direction::VERTICAL, 4
        },
        {
            {
                { 'F', SQR(Sq::J9 ) },
                { 'l', SQR(Sq::J10) }, // NOTE: blank
                // { 'Y', SQR(Sq::J11) },
            },
            Player::Player1, 8, Sq::J9, Direction::HORIZONTAL, 3
        },
        {
            {
                { 'R', SQR(Sq::K12) },
                { 'A', SQR(Sq::L12) },
                { 'G', SQR(Sq::M12) },
                { 'E', SQR(Sq::N12) },
            },
            Player::Player2, 12, Sq::K12, Direction::VERTICAL, 4
        },
        {
            {
                // { 'G', SQR(Sq::M12) },
                { 'O', SQR(Sq::M13) },
            },
            Player::Player1, 6, Sq::M12, Direction::HORIZONTAL, 2
        },
    };

    for (auto&& [gmove, player, score, square, direction, length] : moves) {
        // std::cerr << "BEFORE:\n" << *board << std::endl;
        auto maybe_move = make_move(*board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        // std::cerr << "AFTER:\n" << *board << "\n" << std::endl;
    }
}

TEST_CASE("ISC Notation")
{
    auto board = std::make_unique<Board>();

    // clang-format off
    std::vector<IscMove> ts = {
        // square spec, word     , score
        { "H8"        , "see"    , 6     },
        { "11H"       , "mayo"   , 24    },
        { "J9"        , "fLy"    , 8     },
        { "12K"       , "rage"   , 12    },
        { "M12"       , "go"     , 6     },
        { "O12"       , "Swan"   , 23    },
        { "15M"       , "son"    , 3     },
        { "13L"       , "to"     , 4     },
        { "11H"       , "mayor"  , 13    },
        { "M12"       , "goos"   , 5     },
        { "8F"        , "qis"    , 12    },
        { "9E"        , "tine"   , 19    },
        { "E9"        , "triad"  , 12    },
        { "12D"       , "lax"    , 20    },
        { "13B"       , "bead"   , 16    },
        { "D12"       , "lac"    , 5     },
        { "15A"       , "aide"   , 25    },
        { "N10"       , "nee"    , 5     },
        { "N9"        , "knee"   , 8     },
        { "O8"        , "fie"    , 26    },
        { "G6"        , "gain"   , 6     },
        { "6D"        , "hung"   , 10    },
        { "D5"        , "shoe"   , 8     },
        { "15A"       , "aided"  , 7     },
        { "7C"        , "cop"    , 14    },
        { "H8"        , "seems"  , 8     },
        { "5A"        , "goes"   , 5     },
        { "A5"        , "griz"   , 42    },
        { "C7"        , "chill"  , 16    },
        { "F4"        , "nan"    , 3     },
        { "4F"        , "now"    , 10    },
        { "H1"        , "brow"   , 27    },
        { "1H"        , "belt"   , 6     },
        { "3G"        , "jo"     , 34    },
        { "3G"        , "jot"    , 11    },
        { "K1"        , "turn"   , 4     },
        { "L4"        , "etude"  , 18    },
        { "7J"        , "mud"    , 6     },
        { "B4"        , "yo"     , 5     },
        { "8K"        , "pet"    , 9     },
        { "J6"        , "am"     , 6     },
    };
    // clang-format on

    for (const auto& isc : ts) {
        INFO("Playing " << isc.sqspec << " " << isc.root << " " << isc.score);
        // std::cerr << "BEFORE:\n" << *board << std::endl;
        auto&& [gmove, player, score, square, direction, length] = make_test_case_from_isc(*board, isc);
        // auto maybe_move = make_move_isc_notation(*board, square_spec, word, score);
        auto maybe_move = make_move(*board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        // std::cerr << "AFTER:\n" << *board << "\n" << std::endl;
    }
}

TEST_CASE("ISC -- insidious v chloso20")
{
    auto board = std::make_unique<Board>();

    // clang-format off
    std::vector<std::string> ts = {
        "H8 frith 30",
        "G5 move 18",
        "10B antherid 66",
        "13H soaring 80",
        "J12 yay 17",
        "F4 dew 32",
        "15D cOAxers 111",
        "4B duked 22",
        "C3 putz 30",
        "11D oil 20",
        "12C owe 30",
        "13A jag 28",
        "A13 jug 33",
        "14F pa 28",
        "H1 airbus 44",
        "3H reorient 68",
        "O1 ditties 27",
        "12L qi 46",
        "2J lean 16",
        "N6 fava 33",
        "D4 kea 20",
        "M9 bo 14",
        "J2 locution 12",
        "K5 mon 22",
        "14J ye 7"
    };
    // clang-format on

    char spec[32];
    char root[32];
    for (const auto& s : ts) {
        IscMove isc;
        sscanf(s.c_str(), "%s %s %d", &spec[0], &root[0], &isc.score);
        isc.sqspec = spec;
        isc.root   = root;
        INFO("Playing " << isc.sqspec << " " << isc.root << " " << isc.score);
        // std::cerr << "BEFORE:\n" << *board << std::endl;
        auto&& [gmove, player, score, square, direction, length] = make_test_case_from_isc(*board, isc);
        auto maybe_move = make_move(*board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        // std::cerr << "AFTER:\n" << *board << "\n" << std::endl;
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

TEST_CASE("ISC -- insidious v cleatier")
{
    auto board = std::make_unique<Board>();

    // clang-format off
    std::vector<std::string> ts = {
        "H7 fou 12",
        "G4 eejit 21",
        "H1 enow 42",
        "I2 area 16",
        "J1 igg 21",
        "J5 mannite 67",
        "11E lavates 40",
        "K4 dine 24",
        "L1 cruor 32",
        "1L chit 27",
        "10D yEz 66",
        "H11 afire 36",
        "14E chirp 20",
        "J13 pya 40",
        "13G xi 34",
        "9C qat 29",
        "D3 interlay 24",
        "12D beg 28",
        "C2 mooed 27",
        "L7 wud 14",
        "15J aubade 36",
        "O8 roselIke 86",
        "6B odea 7",
    };
    // clang-format on

    for (const auto& s : ts) {
        auto isc = _parse_isc_string(s);
        INFO("Playing " << isc.sqspec << " " << isc.root << " " << isc.score);
        // std::cerr << "BEFORE:\n" << *board << std::endl;
        auto&& [gmove, player, score, square, direction, length] = make_test_case_from_isc(*board, isc);
        auto maybe_move = make_move(*board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);
        // std::cerr << "AFTER:\n" << *board << "\n" << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const std::vector<Word>& words)
{
    os << "{ ";
    for (const auto& word : words) {
        os << word << " ";
    }
    os << "}";
    return os;
}

bool operator==(const std::vector<Word>& lhs, const std::vector<Word>& rhs)
{
    assert(std::is_sorted(lhs.begin(), lhs.end()));
    assert(std::is_sorted(rhs.begin(), rhs.end()));
    std::vector<Word> diffs;
    std::set_symmetric_difference(lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end(),
            std::back_inserter(diffs));
    // std::cerr << "DIFFS" << diffs << std::endl;
    return diffs.empty();
}

bool operator!=(const std::vector<Word>& lhs, const std::vector<Word>& rhs) {
    return !(lhs == rhs);
}

TEST_CASE("Find all words")
{
    auto board = std::make_unique<Board>();
    // clang-format off
    std::vector<std::pair<std::string, std::vector<Word>>> ts = {
        { "8D areca 16",     { "ARECA" } },
        { "D7 fauvist 26",   { "FAUVIST" } },
        { "F6 zoea 33",      { "ZOEA" } },
        { "10F lipoma 34",   { "ZOEAL", "LIPOMA" } },
        { "6F za 11",        { "ZA" } },
        { "K5 outhear 40",   { "OUTHEAR" } },
        { "12F dearest 79",  { "DEAREST", "OUTHEARS" } },
        { "9I jeed 41",      { "JO", "EM", "JEED" } },
        { "C3 aioli 18",     { "AIOLI", "IF" } },
        { "D1 karn 32",      { "KARN", "AR", "IN" } },
        { "1A brokenlY 167", { "BROKENLY" } },
        { "H12 ably 27",     { "ABLY" } },
        { "8K hedge 37",     { "HEDGE", "ED" } },
        { "L1 wingy 37",     { "WINGY", "OY" } },
        { "3G footing 32",   { "FOOTING" } },
        { "4F new 24",       { "NEW", "FE", "OW" } },
        { "2K qi 22",        { "QI", "QI" } },
        { "14B vest 28",     { "VEST", "FAUVISTS" } },
        { "O1 hexamine 113", { "HEXAMINE" } },
        { "N8 guidons 22",   { "GUIDONS" } },
        { "B5 pAretic 73",   { "PARETIC", "PO", "AL", "RIF" } },
    };
    // clang-format on

    for (auto&& [s, expected_words] : ts) {
        auto isc = _parse_isc_string(s);
        INFO("Playing " << isc.sqspec << " " << isc.root << " " << isc.score);
        // std::cerr << "BEFORE:\n" << *board << std::endl;
        auto&& [gmove, player, score, square, direction, length] = make_test_case_from_isc(*board, isc);
        auto maybe_move = make_move(*board, gmove);
        REQUIRE(static_cast<bool>(maybe_move) == true);
        auto move = *maybe_move;
        CHECK(move.player == player);
        CHECK(move.score == score);
        CHECK(move.square == square);
        CHECK(move.direction == direction);
        CHECK(move.length == length);

        auto result = find_words(*board, move);
        std::sort(result.begin(), result.end());
        std::sort(expected_words.begin(), expected_words.end());
        REQUIRE(result == expected_words);
        // std::cerr << "AFTER:\n" << *board << "\n" << std::endl;
    }
}
