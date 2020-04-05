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
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>

#include "tables.h"

#define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__);

using Dict = std::unordered_set<std::string>;

template <class T>
using RackArray = std::array<T, NumTilesRack>;

#define AsIndex(x) static_cast<std::size_t>(x)

enum class Direction : int {
    HORIZONTAL = Right,
    VERTICAL = Down,
};

enum class Player : int {
    Player1 = 0,
    Player2 = 1,
};

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

bool operator==(const Board& lhs, const Board& rhs) noexcept {
    return lhs.n_moves == rhs.n_moves && lhs.brd == rhs.brd;
}

bool operator!=(const Board& lhs, const Board& rhs) noexcept { return !(lhs == rhs); }

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

// clang-format off
struct Move
{
    Player    player;
    Score     score;
    Square    square;
    Direction direction;
    int       length;
    Tiles     tiles;

    // TEMP TEMP
    std::vector<std::string> words_formed;
};
// clang-format on

template <class Iter, class F>
bool all_same(Iter first, Iter last, F f) {
    auto&& val = f(*first++);
    while (first != last) {
        if (val != f(*first++)) {
            return false;
        }
    }
    return true;
}

constexpr int getrow(Square s) noexcept { return s / Dim; }
constexpr int getcol(Square s) noexcept { return s % Dim; }

template <class Iter>
bool all_unique(Iter first, Iter last) {
    assert(std::is_sorted(first, last));
    return std::unique(first, last) == last;
}

std::optional<Move> make_move(Board& b, const GuiMove& m) {
    // New words may be formed by:
    //  + adding one or more letters to a word or letters already on the board
    //  + must read either across or down; diagonal words are not allowed
    //  + Placing a word at right angles to a word already on the board.
    //    The new word must use one of the ltters already on the board or
    //    must add a letter to it.
    //  + Placing a complete word parallel to a word already played so that
    //    adjacent letters also form complete words.

    auto& board = b.brd;

    if (!(1 <= m.size() && m.size() <= NumTilesRack)) {
        DEBUG("error: invalid number of tiles: %zu", m.size());
        return std::nullopt;
    }

    int n_tiles = static_cast<int>(m.size());
    Tiles tiles;
    RackArray<Square> squares;
    RackArray<int> indices;
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.begin() + n_tiles,
              [&m](int lhs, int rhs) { return m[lhs].second < m[rhs].second; });
    std::fill(tiles.begin(), tiles.end(), Empty);
    std::fill(squares.begin(), squares.end(), InvalidSquare);
    for (int i = 0; i < n_tiles; ++i) {
        tiles[i] = m[indices[i]].first;
        squares[i] = m[indices[i]].second;
    }

    const auto squares_begin = squares.begin();
    const auto squares_end = squares.begin() + n_tiles;
    const auto tiles_begin = tiles.begin();
    const auto tiles_end = tiles.begin() + n_tiles;

    if (!all_unique(squares_begin, squares_end)) {
        DEBUG("error: squares are not unique");
        return std::nullopt;
    }

    bool same_row = all_same(squares_begin, squares_end, getrow);
    bool same_col = all_same(squares_begin, squares_end, getcol);
    if (!same_row && !same_col) {
        DEBUG("error: squares are not all on the same row or column");
        return std::nullopt;
    }
    assert((same_row || same_col) || (same_row && same_col && m.size() == 1));

    if (b.n_moves > 0) {
        if (std::none_of(squares_begin, squares_end, [&board](Square square) -> bool {
                auto row = getrow(square);
                auto col = getcol(square);
                if (col - 1 >= 0 && board[square + Left] != Empty) {
                    return true;
                }
                if (col + 1 < Dim && board[square + Right] != Empty) {
                    return true;
                }
                if (row - 1 >= 0 && board[square + Up] != Empty) {
                    return true;
                }
                if (row + 1 < Dim && board[square + Down] != Empty) {
                    return true;
                }
                return false;
            })) {
            DEBUG("error: no tiles add to word already on board");
            return std::nullopt;
        }
    }

    for (int i = 0; i < n_tiles; ++i) {
        const auto square = squares[i];
        const auto tile = tiles[i];
        if (board[square] != Empty) {
            for (int j = i - 1; j >= 0; --j) {
                board[square] = Empty;
            }
            DEBUG("error: square not available: %s (%d)", SquareNames[square], square);
            return std::nullopt;
        }
        board[square] = tile;
    }

    {  // TEMP TEMP
        DEBUG("Dumping sorted tiles:");
        for (int i = 0; i < NumTilesRack; ++i) {
            const auto square = squares[i];
            const auto tile = tiles[i];
            DEBUG("tile='%c' square=%s (%d)", tile, SquareNames[square], square);
        }
    }

    auto _undo_move_and_return_null = [&board, &m]() {
        for (auto&& [tile, square] : m) {
            board[square] = Empty;
        }
        return std::nullopt;
    };

    // Have verified that:
    //   1) all squares being played are unique
    //   2) all squares being played are on the same row or column
    //   3) all squares being played were previously empty on the board
    //
    // Need to verify that:
    //   1) if first move, H8 must be occupied
    //   2) play connects to a previous play if not first move
    //   3) play is contiguous in one direction
    //   4) all words formed are valid
    //
    // Need to determine:
    //   1) starting square
    //   2) direction
    //   3) score

    if (board[Sq_H8] == Empty) {
        DEBUG("error: did not occupy H8 square on first move");
        return _undo_move_and_return_null();
    }

    Move result;
    result.player = b.n_moves % 2 == 0 ? Player::Player1 : Player::Player2;
    result.score = 0;
    result.square = InvalidSquare;
    // result.direction = /* INVALID */;
    result.length = 0;
    result.tiles = std::move(tiles);

    const int first_tile_sq = squares[0];
    const int first_tile_row = getrow(first_tile_sq);
    const int first_tile_col = getcol(first_tile_sq);
    const int last_tile_sq = *(squares_end - 1);
    if (same_row) {
        const int row_start = first_tile_row * Dim;  // inclusive
        const int row_stop = row_start + Dim;        // exclusive
        int horz_start = first_tile_sq + Left;
        while (horz_start >= row_start && board[horz_start] != Empty) {
            horz_start += Left;
        }
        horz_start += Right;
        assert(board[horz_start] != Empty);

        int horz_stop = first_tile_sq + Right;
        while (horz_stop < row_stop && board[horz_stop] != Empty) {
            horz_stop += Right;
        }
        assert(horz_start < horz_stop);
        DEBUG("horz=[%d, %d) [%s, %s]", horz_start, horz_stop, SquareNames[horz_start], SquareNames[horz_stop + Left]);

        if (last_tile_sq >= horz_stop) {
            DEBUG("error: tiles are not contiguous horizontally: last_tile=%d horz_stop=%d", last_tile_sq, horz_stop);
            return _undo_move_and_return_null();
        }

        result.square = horz_start;
        result.direction = Direction::HORIZONTAL;
        result.length = horz_stop - horz_start;
    }

    if (same_col) {
        const int col_start = first_tile_col;  // inclusive
        const int col_stop = NumSquares;       // exclusive
        int vert_start = first_tile_sq + Up;
        while (vert_start >= col_start && board[col_start] != Empty) {
            vert_start += Up;
        }
        vert_start += Down;
        assert(board[vert_start] != Empty);

        int vert_stop = first_tile_sq + Down;
        while (vert_stop < col_stop && board[vert_stop] != Empty) {
            vert_stop += Down;
        }
        assert(vert_start < vert_stop);
        DEBUG("vert=[%d, %d) [%s, %s]", vert_start, vert_stop, SquareNames[vert_start], SquareNames[vert_stop + Up]);

        if (last_tile_sq >= vert_stop) {
            DEBUG("error: tiles are not contiguous vertically");
            return _undo_move_and_return_null();
        }

        int length = (vert_stop - vert_start) / Dim;
        if (length > result.length) {
            result.square = vert_start;
            result.direction = Direction::VERTICAL;
            result.length = length;
        }
    }

    b.n_moves++;
    return result;
}

#if 0
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

    template <class Iter>
    static bool is_unique(Iter a, Iter b) {

    }

    // TODO: might be easier to let this function place the pieces on the
    // board, and remove them if it is bad / just save off a copy of the board
    // state before.
    static std::optional<Move> make(const Board& board, GuiMove& move) noexcept {
        // New words may be formed by:
        //  + adding one or more letters to a word or letters already on the board
        //  + Placing a word at right angles to a word already on the board.
        //    The new word must use one of the ltters already on the board or
        //    must add a letter to it.
        //  + Placing a complete word parallel to a word already played so that
        //    adjacent letters also form complete words.

        if (move.empty() || move.size() > MaxWordLength) {
            DEBUG("word length invalid: %zu", move.size());
            return std::nullopt;
        }

        const auto& brd = board.brd;

        // no duplicate squares, and all squares are empty on the board
        std::set<Square> seen;
        for (auto&& [tile, square] : move) {
            if (brd[AsIndex(square)] != Empty) {
                DEBUG("square=%d occupied=%c", square, brd[AsIndex(square)]);
                return std::nullopt;
            }
            if (seen.find(square) != seen.end()) {
                DEBUG("duplicate square=%d", square);
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
            if (tile_it != tile_end_it) {
                stop_sq_horz = start_sq_horz;
            } else {
                assert(brd[stop_sq_horz+Left] != Empty || move.back().second == stop_sq_horz+Left);
            }
        }

        // find longest word length starting at `start_sq_vert` going down
        // using non-empty board squares and tiles
        int stop_sq_vert = -1;
        {
            auto tile_it = std::begin(move);
            auto tile_end_it = std::end(move);
            assert(vert_start <= start_sq_vert && start_sq_vert < vert_stop);
            assert((start_sq_vert % Dim) == col);
            for (stop_sq_vert = start_sq_vert; stop_sq_vert < vert_stop; stop_sq_vert += Down) {
                if (tile_it != tile_end_it && tile_it->second == stop_sq_vert) {
                    assert(brd[stop_sq_vert] == Empty);
                    ++tile_it;
                } else if (brd[stop_sq_vert] == Empty) {
                    break;
                }
            }
            if (tile_it != tile_end_it) {
                stop_sq_vert = start_sq_vert;
            } else {
                assert(brd[stop_sq_vert+Up] != Empty || move.back().second == stop_sq_vert+Up);
            }
        }

        DEBUG("horz=[%d,%d] vert=[%d,%d]", start_sq_horz, stop_sq_horz, start_sq_vert, stop_sq_vert);

        const int len_horz = stop_sq_horz - start_sq_horz;
        const int len_vert = (stop_sq_vert - start_sq_vert) / Dim;
        if (len_horz < 2 && len_vert < 2) {
            DEBUG("word not long enough in either direction: horz=%d vert=%d", len_horz, len_vert);
            return std::nullopt;  // didn't use all tiles in either direction
        }

        const auto direction = len_horz >= len_vert ? Direction::HORIZONTAL : Direction::VERTICAL;
        const auto first_square = len_horz >= len_vert ? start_sq_horz : start_sq_vert;
        const auto max_length = len_horz >= len_vert ? len_horz : len_vert;
        if (board.n_moves > 0 && AsIndex(max_length) == move.size()) {  // no contiguous board tiles
            DEBUG("no contiguous board tiles: len_horz=%d len_vert=%d move=%zu", len_horz, len_vert, move.size());
            return std::nullopt;
        } else if (board.n_moves == 0) {  // must play through H8 on move 1
            DEBUG("H8 not played on move 1");
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
#endif

#if 0
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
    board.n_moves++;
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
