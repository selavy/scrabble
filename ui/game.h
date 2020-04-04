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
#include <optional>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>

constexpr int Dim = 15;
constexpr int NumSquares = Dim * Dim;
constexpr int NumBlankTiles = 2;
constexpr int MinWordLength = 2;
constexpr int MaxWordLength = Dim;
constexpr char Empty = ' ';
constexpr char Blank = '?';
constexpr int Left = -1;
constexpr int Right = 1;
constexpr int Up = -Dim;
constexpr int Down = Dim;

constexpr int Sq_A1 = 0;
constexpr int Sq_A2 = 1;
constexpr int Sq_A3 = 2;
constexpr int Sq_A4 = 3;
constexpr int Sq_A5 = 4;
constexpr int Sq_A6 = 5;
constexpr int Sq_A7 = 6;
constexpr int Sq_A8 = 7;
constexpr int Sq_A9 = 8;
constexpr int Sq_A10 = 9;
constexpr int Sq_A11 = 10;
constexpr int Sq_A12 = 11;
constexpr int Sq_A13 = 12;
constexpr int Sq_A14 = 13;
constexpr int Sq_A15 = 14;
constexpr int Sq_B1 = 15;
constexpr int Sq_B2 = 16;
constexpr int Sq_B3 = 17;
constexpr int Sq_B4 = 18;
constexpr int Sq_B5 = 19;
constexpr int Sq_B6 = 20;
constexpr int Sq_B7 = 21;
constexpr int Sq_B8 = 22;
constexpr int Sq_B9 = 23;
constexpr int Sq_B10 = 24;
constexpr int Sq_B11 = 25;
constexpr int Sq_B12 = 26;
constexpr int Sq_B13 = 27;
constexpr int Sq_B14 = 28;
constexpr int Sq_B15 = 29;
constexpr int Sq_C1 = 30;
constexpr int Sq_C2 = 31;
constexpr int Sq_C3 = 32;
constexpr int Sq_C4 = 33;
constexpr int Sq_C5 = 34;
constexpr int Sq_C6 = 35;
constexpr int Sq_C7 = 36;
constexpr int Sq_C8 = 37;
constexpr int Sq_C9 = 38;
constexpr int Sq_C10 = 39;
constexpr int Sq_C11 = 40;
constexpr int Sq_C12 = 41;
constexpr int Sq_C13 = 42;
constexpr int Sq_C14 = 43;
constexpr int Sq_C15 = 44;
constexpr int Sq_D1 = 45;
constexpr int Sq_D2 = 46;
constexpr int Sq_D3 = 47;
constexpr int Sq_D4 = 48;
constexpr int Sq_D5 = 49;
constexpr int Sq_D6 = 50;
constexpr int Sq_D7 = 51;
constexpr int Sq_D8 = 52;
constexpr int Sq_D9 = 53;
constexpr int Sq_D10 = 54;
constexpr int Sq_D11 = 55;
constexpr int Sq_D12 = 56;
constexpr int Sq_D13 = 57;
constexpr int Sq_D14 = 58;
constexpr int Sq_D15 = 59;
constexpr int Sq_E1 = 60;
constexpr int Sq_E2 = 61;
constexpr int Sq_E3 = 62;
constexpr int Sq_E4 = 63;
constexpr int Sq_E5 = 64;
constexpr int Sq_E6 = 65;
constexpr int Sq_E7 = 66;
constexpr int Sq_E8 = 67;
constexpr int Sq_E9 = 68;
constexpr int Sq_E10 = 69;
constexpr int Sq_E11 = 70;
constexpr int Sq_E12 = 71;
constexpr int Sq_E13 = 72;
constexpr int Sq_E14 = 73;
constexpr int Sq_E15 = 74;
constexpr int Sq_F1 = 75;
constexpr int Sq_F2 = 76;
constexpr int Sq_F3 = 77;
constexpr int Sq_F4 = 78;
constexpr int Sq_F5 = 79;
constexpr int Sq_F6 = 80;
constexpr int Sq_F7 = 81;
constexpr int Sq_F8 = 82;
constexpr int Sq_F9 = 83;
constexpr int Sq_F10 = 84;
constexpr int Sq_F11 = 85;
constexpr int Sq_F12 = 86;
constexpr int Sq_F13 = 87;
constexpr int Sq_F14 = 88;
constexpr int Sq_F15 = 89;
constexpr int Sq_G1 = 90;
constexpr int Sq_G2 = 91;
constexpr int Sq_G3 = 92;
constexpr int Sq_G4 = 93;
constexpr int Sq_G5 = 94;
constexpr int Sq_G6 = 95;
constexpr int Sq_G7 = 96;
constexpr int Sq_G8 = 97;
constexpr int Sq_G9 = 98;
constexpr int Sq_G10 = 99;
constexpr int Sq_G11 = 100;
constexpr int Sq_G12 = 101;
constexpr int Sq_G13 = 102;
constexpr int Sq_G14 = 103;
constexpr int Sq_G15 = 104;
constexpr int Sq_H1 = 105;
constexpr int Sq_H2 = 106;
constexpr int Sq_H3 = 107;
constexpr int Sq_H4 = 108;
constexpr int Sq_H5 = 109;
constexpr int Sq_H6 = 110;
constexpr int Sq_H7 = 111;
constexpr int Sq_H8 = 112;
constexpr int Sq_H9 = 113;
constexpr int Sq_H10 = 114;
constexpr int Sq_H11 = 115;
constexpr int Sq_H12 = 116;
constexpr int Sq_H13 = 117;
constexpr int Sq_H14 = 118;
constexpr int Sq_H15 = 119;
constexpr int Sq_I1 = 120;
constexpr int Sq_I2 = 121;
constexpr int Sq_I3 = 122;
constexpr int Sq_I4 = 123;
constexpr int Sq_I5 = 124;
constexpr int Sq_I6 = 125;
constexpr int Sq_I7 = 126;
constexpr int Sq_I8 = 127;
constexpr int Sq_I9 = 128;
constexpr int Sq_I10 = 129;
constexpr int Sq_I11 = 130;
constexpr int Sq_I12 = 131;
constexpr int Sq_I13 = 132;
constexpr int Sq_I14 = 133;
constexpr int Sq_I15 = 134;
constexpr int Sq_J1 = 135;
constexpr int Sq_J2 = 136;
constexpr int Sq_J3 = 137;
constexpr int Sq_J4 = 138;
constexpr int Sq_J5 = 139;
constexpr int Sq_J6 = 140;
constexpr int Sq_J7 = 141;
constexpr int Sq_J8 = 142;
constexpr int Sq_J9 = 143;
constexpr int Sq_J10 = 144;
constexpr int Sq_J11 = 145;
constexpr int Sq_J12 = 146;
constexpr int Sq_J13 = 147;
constexpr int Sq_J14 = 148;
constexpr int Sq_J15 = 149;
constexpr int Sq_K1 = 150;
constexpr int Sq_K2 = 151;
constexpr int Sq_K3 = 152;
constexpr int Sq_K4 = 153;
constexpr int Sq_K5 = 154;
constexpr int Sq_K6 = 155;
constexpr int Sq_K7 = 156;
constexpr int Sq_K8 = 157;
constexpr int Sq_K9 = 158;
constexpr int Sq_K10 = 159;
constexpr int Sq_K11 = 160;
constexpr int Sq_K12 = 161;
constexpr int Sq_K13 = 162;
constexpr int Sq_K14 = 163;
constexpr int Sq_K15 = 164;
constexpr int Sq_L1 = 165;
constexpr int Sq_L2 = 166;
constexpr int Sq_L3 = 167;
constexpr int Sq_L4 = 168;
constexpr int Sq_L5 = 169;
constexpr int Sq_L6 = 170;
constexpr int Sq_L7 = 171;
constexpr int Sq_L8 = 172;
constexpr int Sq_L9 = 173;
constexpr int Sq_L10 = 174;
constexpr int Sq_L11 = 175;
constexpr int Sq_L12 = 176;
constexpr int Sq_L13 = 177;
constexpr int Sq_L14 = 178;
constexpr int Sq_L15 = 179;
constexpr int Sq_M1 = 180;
constexpr int Sq_M2 = 181;
constexpr int Sq_M3 = 182;
constexpr int Sq_M4 = 183;
constexpr int Sq_M5 = 184;
constexpr int Sq_M6 = 185;
constexpr int Sq_M7 = 186;
constexpr int Sq_M8 = 187;
constexpr int Sq_M9 = 188;
constexpr int Sq_M10 = 189;
constexpr int Sq_M11 = 190;
constexpr int Sq_M12 = 191;
constexpr int Sq_M13 = 192;
constexpr int Sq_M14 = 193;
constexpr int Sq_M15 = 194;
constexpr int Sq_N1 = 195;
constexpr int Sq_N2 = 196;
constexpr int Sq_N3 = 197;
constexpr int Sq_N4 = 198;
constexpr int Sq_N5 = 199;
constexpr int Sq_N6 = 200;
constexpr int Sq_N7 = 201;
constexpr int Sq_N8 = 202;
constexpr int Sq_N9 = 203;
constexpr int Sq_N10 = 204;
constexpr int Sq_N11 = 205;
constexpr int Sq_N12 = 206;
constexpr int Sq_N13 = 207;
constexpr int Sq_N14 = 208;
constexpr int Sq_N15 = 209;
constexpr int Sq_O1 = 210;
constexpr int Sq_O2 = 211;
constexpr int Sq_O3 = 212;
constexpr int Sq_O4 = 213;
constexpr int Sq_O5 = 214;
constexpr int Sq_O6 = 215;
constexpr int Sq_O7 = 216;
constexpr int Sq_O8 = 217;
constexpr int Sq_O9 = 218;
constexpr int Sq_O10 = 219;
constexpr int Sq_O11 = 220;
constexpr int Sq_O12 = 221;
constexpr int Sq_O13 = 222;
constexpr int Sq_O14 = 223;
constexpr int Sq_O15 = 224;

#define AsIndex(x) static_cast<std::size_t>(x)

using Dict = std::unordered_set<std::string>;
using Tile = char;
using Square = int;
using Tiles = std::array<Tile, MaxWordLength>;
// clang-format off
constexpr Tiles all_empty_tiles = {
    Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
    Empty, Empty, Empty, Empty, Empty, Empty, Empty,
};
// clang-format on

constexpr int ix(char row, int col) {
    assert('A' <= row && row <= 'O');
    assert(1 <= col && col <= Dim);
    return (row - 'A') * Dim + (col - 1);
}

struct Board {
    std::array<char, NumSquares> brd;
    int n_moves = 0;
    Board() noexcept { std::fill(std::begin(brd), std::end(brd), Empty); }
};

std::ostream& operator<<(std::ostream& os, const Board& board) {
    auto print_row = [&os](const char* r) {
        for (int i = 0; i < Dim - 1; ++i) {
            os << r[i] << " | ";
        }
        os << r[Dim - 1];
    };
    const auto& b = board.brd;
    os << "     1   2   3   4   5   6   7   8   9   0   1   2   3   4   5  \n";
    os << "   -------------------------------------------------------------\n";
    for (int row = 0; row < Dim; ++row) {
        os << static_cast<char>('A' + row) << "  | ";
        print_row(&b[Dim * row]);
        os << " |\n";
        os << "   -------------------------------------------------------------\n";
    }
    return os;
}

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

    using Letters = std::array<char, MaxWordLength + 1>;
    Letters letters = {'\0'};  // null terminated c-string format and all caps
    int length = 0;

    constexpr Word(const std::string& word) noexcept {
        assert(word.size() <= static_cast<std::size_t>(MaxWordLength));
        length = static_cast<int>(word.size());
        for (std::size_t i = 0; i < word.size(); ++i) {
            assert('A' <= word[i] && word[i] <= 'Z');
            letters[i] = word[i];
        }
        assert(strlen(&letters[0]) == length);
        assert(MinWordLength <= length && length <= MaxWordLength);
    }

    constexpr Word(const Word& other) noexcept : letters{other.letters}, length{other.length} {}

    constexpr Word(Word&& other) noexcept : letters{other.letters}, length{other.length} {
        // TODO(peter): temp temp -- don't actually need to clear other.letters
        std::fill(std::begin(other.letters), std::end(other.letters), '\0');
        other.length = 0;
    }

    constexpr int size() const noexcept { return length; }
    constexpr const char* c_str() const noexcept { return &letters[0]; }
    std::string str() const noexcept { return {&letters[0], &letters[length]}; }

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

using GuiMove = std::vector<std::pair<Tile, Square>>;

struct Move {
    constexpr static int MinSquareNum = 0;
    constexpr static int MaxSquareNum = NumSquares;

    // clang-format off
    Direction direction;
    int       square;
    Tiles     tiles;
    int       max_word_length;
    // clang-format on

    template <class Iter>
    static bool all_same(Iter first, Iter last) {
        Iter cur = first;
        while (cur != last) {
            if (*cur != *first) return false;
            ++cur;
        }
        return true;
    }

    // TODO: might be easier to let this function place the pieces on the
    // board, and remove them if it is bad / just save off a copy of the board
    // state before.
    static std::optional<Move> make(const Board& board, GuiMove& move) noexcept {
        if (move.empty() || move.size() > MaxWordLength) {
            return std::nullopt;
        }

        const auto& brd = board.brd;

        // no duplicate squares, and all squares are empty on the board
        std::set<Square> seen;
        for (auto&& [tile, square] : move) {
            if (brd[AsIndex(square)] != Empty) {
                return std::nullopt;
            }
            if (seen.find(square) != seen.end()) {
                return std::nullopt;
            }
            seen.insert(square);
        }

        // make first square the upper left most tile played moving to the
        // lower right
        std::sort(
            std::begin(move), std::end(move),
            [](const std::pair<Tile, Square>& p1, const std::pair<Tile, Square>& p2) { return p1.second < p2.second; });

        auto&& [tile, square] = move[0];
        const int row = square / Dim;
        const int col = square % Dim;
        const int horz_start = Dim * row;           // inclusive
        const int horz_stop = horz_start + Dim;     // exclusive
        const int vert_start = col;                 // inclusive
        const int vert_stop = Dim * Dim /*+ col*/;  // exclusive

        // find left-most contiguous tile on board
        int start_sq_horz = square + Left;
        while (start_sq_horz >= horz_start && brd[start_sq_horz] != Empty) {
            start_sq_horz += Left;
        }
        start_sq_horz += Right;

        // find north-most contiguous tile on board
        int start_sq_vert = square + Up;
        while (start_sq_vert >= vert_start && brd[start_sq_vert] != Empty) {
            start_sq_vert += Up;
        }
        start_sq_vert += Down;

        // find longest word length starting at `start_sq_horz` going right
        // using non-empty board squares and tiles
        int stop_sq_horz = -1;
        {
            auto tile_it = std::begin(move);
            auto tile_end_it = std::end(move);
            assert(horz_start <= start_sq_horz && start_sq_horz < horz_stop);
            assert((start_sq_horz / Dim) == row);
            for (stop_sq_horz = start_sq_horz; stop_sq_horz < horz_stop; stop_sq_horz += Right) {
                if (tile_it != tile_end_it && tile_it->second == stop_sq_horz) {
                    assert(brd[stop_sq_horz] == Empty);
                    ++tile_it;
                } else if (brd[stop_sq_horz] == Empty) {
                    break;
                }
            }
            stop_sq_horz += Left;
            if (tile_it != tile_end_it) {
                stop_sq_horz = start_sq_horz;
            } else {
                assert(brd[stop_sq_horz] != Empty || move.back().second == stop_sq_horz);
            }
        }

        // find longest word length starting at `start_sq_vert` going down
        // using non-empty board squares and tiles
        int stop_sq_vert = -1;
        {
            auto tile_it = std::begin(move);
            auto tile_end_it = std::end(move);
            assert(vert_start <= start_sq_vert && start_sq_vert < vert_stop);
            assert((start_sq_vert / Dim) == row);
            for (stop_sq_vert = start_sq_vert; stop_sq_vert < vert_stop; stop_sq_vert += Down) {
                if (tile_it != tile_end_it && tile_it->second == stop_sq_vert) {
                    assert(brd[stop_sq_vert] == Empty);
                    ++tile_it;
                } else if (brd[stop_sq_vert] == Empty) {
                    break;
                }
            }
            stop_sq_vert += Up;
            if (tile_it != tile_end_it) {
                stop_sq_vert = start_sq_vert;
            } else {
                assert(brd[stop_sq_vert] != Empty || move.back().second == stop_sq_vert);
            }
        }

        const int len_horz = (stop_sq_horz - start_sq_horz) + 1;
        const int len_vert = (stop_sq_vert - start_sq_vert) + 1;
        if (len_horz < 2 && len_vert < 2) {
            return std::nullopt;  // didn't use all tiles in either direction
        }

        const auto direction = len_horz >= len_vert ? Direction::HORIZONTAL : Direction::VERTICAL;
        const auto first_square = len_horz >= len_vert ? start_sq_horz : start_sq_vert;
        const auto max_length = len_horz >= len_vert ? len_horz : len_vert;
        if (board.n_moves > 0 && AsIndex(max_length) == move.size()) {  // no contiguous board tiles
            return std::nullopt;
        } else if (board.n_moves == 0) {  // must play through H8 on move 1
            auto h8_it = std::find_if(std::begin(move), std::end(move),
                                      [](const std::pair<Tile, Square>& p) { return p.second == Sq_H8; });
            if (h8_it == std::end(move)) {
                return std::nullopt;
            }
        }
        Tiles tiles = all_empty_tiles;
        for (size_t i = 0; i < move.size(); ++i) {
            tiles[i] = move[i].first;
        }

        Move result{direction, first_square, tiles, max_length};
        // return { std::in_place, direction, first_square, tiles };
        return result;
    }

    constexpr Move(Direction direction_, int square_, Tiles tiles_, int max_word_length_) noexcept
        : direction{direction_}, square{square_}, tiles{tiles_}, max_word_length{max_word_length_} {
#ifndef NDEBUG
        assert(MinSquareNum <= square && square <= MaxSquareNum);
        // TODO(peter): `std::count_if` is not constexpr?
        // int n_blanks = std::count_if(
        //                     std::begin(tiles),
        //                     std::end(tiles),
        //                     [](Tile c) -> bool { return 'a' <= c && c <= 'z'; }
        //                );
        int n_blanks = 0;
        for (auto tile : tiles) n_blanks += ('a' <= tile && tile <= 'z') ? 1 : 0;
        assert(n_blanks <= NumBlankTiles);
        // TODO(peter): `std::find` is not constexpr?
        // auto first_empty = std::find(std::begin(tiles), std::end(tiles), Empty);
        // auto next_non_empty = std::find_if_not(first_empty, std::end(tiles), Empty);
        auto first_empty = std::begin(tiles);
        for (; first_empty != std::end(tiles) && *first_empty != Empty; ++first_empty)
            ;
        auto next_non_empty = first_empty;
        for (; next_non_empty != std::end(tiles) && *next_non_empty == Empty; ++next_non_empty)
            ;
        assert(next_non_empty == std::end(tiles));
#endif
    }
};

// TODO(peter): I am thinking that applying during move creation is going to be
// better because I'm having to recalculate information here that I already had
void make_move(Board& board, const Move move) {
    const int step = static_cast<int>(move.direction);
    const int start = move.square;
    const int stop = start + step * move.max_word_length;  // exclusive
    int tidx = 0;
    for (int sq = start; sq < stop; sq += step) {
        if (board.brd[sq] != Empty) {
            continue;
        }
        assert(AsIndex(tidx) < move.tiles.size());
        assert(move.tiles[tidx] != Empty);
        board.brd[sq] = move.tiles[tidx++];
    }
}

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
