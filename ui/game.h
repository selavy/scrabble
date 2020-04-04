#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

using Dict = std::unordered_set<std::string>;

constexpr int Dim = 15;
constexpr int NumSquares = Dim * Dim;
constexpr int NumBlankTiles = 2;
constexpr char Empty = ' ';
constexpr char Blank = '?';

struct Board {
    std::array<char, NumSquares> brd;
    int moves = 0;
    Board() noexcept { std::fill(std::begin(brd), std::end(brd), Empty); }
};

enum class Direction : int {
    HORIZONTAL = 1,
    VERTICAL = 10,
};

enum class Player : int {
    Player1 = 0,
    Player2 = 1,
};
constexpr Player flip_player(Player p) noexcept {
    auto v = static_cast<int>(p);
    return static_cast<Player>(v ^ 1);
}

struct Word {
    // blank or not            => 1 bit / letter for blank or not
    // 26 letters + 1 empty(?) => 5 bits / letter
    // 1-15 length             => 4 bits for length

    constexpr static int MaxWordLength = 15;
    // using Tiles = std::array<char, MaxWordLength>;
    using Letters = std::array<char, MaxWordLength + 1>;

    // // clang-format off
    // constexpr static Tiles all_empty_tiles = {
    //     Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
    //     Empty, Empty, Empty, Empty, Empty, Empty, Empty,
    // };
    // // clang-format on

    // Tiles tiles = all_empty_tiles;
    Letters letters = {'\0'};  // null terminated c-string format and all caps
    int length = 0;

    constexpr Word(const std::string& word) noexcept {
        assert(word.size() <= static_cast<std::size_t>(MaxWordLength));
        length = static_cast<int>(word.size());
        for (std::size_t i = 0; i < word.size(); ++i) {
            assert('A' <= word[i] && word[i] <= 'Z');
            letters[i] = tiles[i] = word[i];
        }
        assert(strlen(&letters[0]) == length);
    }

    constexpr Word(const Word& other) noexcept : letters{other.letters}, length{other.length} {}

    constexpr Word(Word&& other) noexcept : letters{other.letters}, length{other.length} {
        // TODO(peter): temp temp -- don't actually need to clear other.letters
        std::fill(std::begin(other.letters), std::end(other.letters), '\0');
        other.length = 0;
    }

    constexpr int size() const noexcept { return length; }
    constexpr const char* c_str() const noexcept { return &letters[0]; }
    constexpr std::string str() const noexcept { return { &letters[0], &letters[length] }; }

    constexpr std::size_t hash() const noexcept {
        uint64_t x1 = 0;
        uint64_t x2 = 0;
        for (size_t i = 0; i < 8; i++) {
            x1 ^= (static_cast<uint64_t>(letters[0 + i]) << (i * 8));
        }
        for (size_t i = 0; i < 7; i++) {
            x2 ^= (static_cast<uint64_t>(letters[8 + i]) << (i * 8));
        }
        return (x1 ^ x2) + 0x9e779b9 + (x1 << 6) + (x2 >> 2);
    }

    bool operator==(Word other) const noexcept {
        return memcmp(&this->letters[0], &other.letters[0], letters.size() * sizeof(char)) == 0;
    }

    bool operator!=(Word other) const noexcept { return !(*this == other); }
};

std::ostream& operator<<(std::ostream& os, const Word& word) {
    assert(word.length <= Word::MaxWordLength);
    os << '\'';
    for (int i = 0; i < word.length; ++i) {
        os << word.letters[i];
    }
    os << '\'';
    return os;
}

namespace std {

template <>
struct hash<Word> {
    std::size_t operator()(const Word& w) const noexcept { return w.hash(); }
};

}  // namespace std

using Dictionary = std::unordered_set<Word>;

#if 0
struct Move {
    constexpr static int MinSquareNum = 0;
    constexpr static int MaxSquareNum = NumSquares;

    // TODO(peter): revisit whether to include player here
    Player player;
    Direction direction;
    int square;
    Word word;

    Move(Player player_, Direction direction_, int square_, Word word_) noexcept
        : player{player_}, direction{direction_}, square{square_}, word{word} {
        assert(!letters_.empty());
        assert(letters_.size() <= static_cast<std::size_t>(MaxMoveLength));
        assert(std::count_if(letters_.begin(), letters_.end(), [](char c) { return c == ' '; }) <= NumBlankTiles);
        length = 0;
        for (auto c : letters_) {
            assert(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'));
            letters[length++] = c;
        }
        assert(static_cast<std::size_t>(length) == letters_.size());
    }
};
constexpr int ix(char row, int col) {
    assert('A' <= row && row <= 'O');
    assert(1 <= col && col <= Dim);
    return (row - 'A') * Dim + (col - 1);
}
#endif

#if 0
// TODO: probably should score move while checking it, return 0 or -1 for invalid
bool is_valid_play(const Dictionary& dict, const Board& board, const Move& move) {
    Word word;

    if ((board.moves % 2) != move.player) {
        return false;
    }

    // check that extends exactly 1 row or column
    int start = move.square;
    int step = static_cast<int>(move.direction);
    int len = move.length;
    int stop = start + len * step;
    if (move.direction == Direction::HORIZONTAL) {
        int start_row = start / Dim;
        int stop_row = stop / Dim;
        if (start_row != stop_row) {
            return false;
        }
    } else if (move.direction == Direction::VERTICAL) {
        int start_col = start % Dim;
        int stop_col = stop % Dim;
        if (start_col != stop_col) {
            return false;
        }
    }

    // // first move must cross through H8
    // if (board.moves == 0) {
    //     constexpr int H8 = ix('H', 8);
    // }

    return true;
}
#endif

// std::unique_ptr<Dict> load_dictionary(const char* filename)
// {
//     std::ifstream ifs(filename);
//     if (!ifs) {
//         return nullptr;
//     }
//     auto dict = std::make_unique<Dict>();
//     std::string word;
//     while (ifs >> word) {
//         if (word.empty() || word.size() > 7) {
//             continue;
//         }
//         dict->emplace(std::move(word));
//     }
//     return dict;
// }
//
// bool is_word(const Dict& dict, char word[7])
// {
//     std::string key;
//     for (size_t i = 0; i < word.size(); ++i) {
//         auto c = word[i];
//         if ('A' <= c && c <= 'Z') {
//             key += c;
//         } else if ('a' <= c && c <= 'z') {
//             key += c;
//         } else {
//             assert(c == ' ');
//             break;
//         }
//     }
//     return dict.find(key) != dict.end();
// }
