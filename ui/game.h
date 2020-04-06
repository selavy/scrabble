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
constexpr Direction flip_direction(Direction x) {
    return x == Direction::HORIZONTAL ? Direction::VERTICAL : Direction::HORIZONTAL;
}

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
    std::array<int, NumSquares + 1> double_letter_squares_ = double_letter_squares;
    std::array<int, NumSquares + 1> triple_letter_squares_ = triple_letter_squares;
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
    Player            player;
    Score             score;
    Square            square;
    Direction         direction;
    int               length;
    Tiles             tiles;
    RackArray<Square> squares;

    // TEMP TEMP
    std::string              root_word;
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

bool contains(const RackArray<Square>& c, Square sq) noexcept {
    return std::find(std::begin(c), std::end(c), sq) != std::end(c);
}

int get_word_multiplier(const RackArray<Square>& squares) noexcept {
    int result = 1;
    for (auto square : squares) {
        result *= double_word_squares[square];
        result *= triple_word_squares[square];
    }
    return result;
}

int get_letter_multiplier(const Board& board, Square square) noexcept {
    return board.double_letter_squares_[square] * board.triple_letter_squares_[square];
}

void clear_multiplier_for_squares(const RackArray<Square>& squares, std::array<int, 226>& mults) {
    for (auto square : squares) {
        mults[square] = 1;
    }
}

// precondition: move is legal to play
void play_move(Board& b, Move& m) noexcept {
    auto& board = b.brd;
    assert((b.n_moves % 2) == static_cast<int>(m.player));

    // play using `tiles` and `squares`
    for (std::size_t i = 0; i < m.squares.size(); ++i) {
        auto square = m.squares[i];
        auto tile = m.tiles[i];
        if (square == InvalidSquare) break;
        assert(board[square] == Empty);
        board[square] = tile;
    }

    // // play using `root_word`
    // const int start = m.square;
    // const int step  = static_cast<int>(m.direction);
    // const int stop  = start + step*DIM;
    // assert (start + step*result.length < stop);
    // for (int i = 0; i < result.length; ++i) {
    //     const int sq = start + i*step;
    //     assert(start <= sq && sq < stop);
    //     if (board[sq] == Empty) {
    //         board[sq] = result.root_word[i];
    //     } else {
    //         assert(board[sq] == result.root_word[i]);
    //     }
    // }

    b.n_moves++;
}

int score_move(const Board& b, /*const*/ Move& m) noexcept {
    int total_score = 0;
    auto& board = b.brd;

    {  // root word score along `m.direction` direction
        const int start = m.square;
        const int step = static_cast<int>(m.direction);
        const int stop = start + step * m.length;
        int score = 0;
        m.root_word.clear();
        for (int sq = start; sq != stop; sq += step) {
            const char letter = board[sq];
            const int value = letter_values[letter];
            const int mult = get_letter_multiplier(b, sq);
            m.root_word += letter;
            score += value * mult;
            DEBUG("ROOT: '%c' -- val=%d mult=%d", letter, value, mult)
        }
        assert(m.root_word.size() >= MinWordLength);
        const int mult = get_word_multiplier(m.squares);
        total_score += score * mult;
    }

    int tiles_played = 0;
    for (const auto root : m.squares) {
        if (root == InvalidSquare) {
            break;
        }
        ++tiles_played;
        const int start = m.direction == Direction::HORIZONTAL ? getcol(root) : Dim * getrow(root);
        const int step = static_cast<int>(flip_direction(m.direction));
        const int stop = start + Dim * step;
        int sq = root - step;
        while (sq >= start && board[sq] != Empty) {
            sq -= step;
        }
        sq += step;
        int score = 0;
        std::string word;
        for (; sq < stop && board[sq] != Empty; sq += step) {
            const char letter = board[sq];
            const int value = letter_values[letter];
            const int mult = get_letter_multiplier(b, sq);
            word += letter;
            score += value * mult;
        }
        if (word.size() > 1) {
            const int mult = double_word_squares[root] * triple_word_squares[root];
            total_score += score * mult;
            DEBUG("THRU WORD: '%s' starting from root '%c' -- %d (mult=%d)", word.c_str(), board[root], score, mult);
        }
    }

    if (tiles_played == 7) {  // Bingo!
        total_score += 50;
    }

    return total_score;
}

std::string convert_to_internal_word(std::string word) {
    for (std::size_t i = 0; i < word.size(); ++i) {
        if ('a' <= word[i] && word[i] <= 'z') {
            word[i] = (word[i] - 'a') + 'A';
        } else if ('A' <= word[i] && word[i] <= 'Z') {
            word[i] = (word[i] - 'A') + 'a';
        } else {
            assert(0 && "invalid character");
        }
    }
    return word;
}

// TODO(peter): using exceptions for now
std::tuple<int, int, Direction> _parse_isc_spec(std::string sqspec) {
    if (sqspec.size() != 2 && sqspec.size() != 3) {
        throw std::runtime_error("invalid ISC square spec");
    }
    int row;
    int col;
    Direction direction;
    if ('A' <= sqspec[0] && sqspec[0] <= 'Z') {
        row = sqspec[0] - 'A';
        if (sqspec.size() == 2) {
            col = (sqspec[1] - '0');
        } else {
            col = (sqspec[1] - '0') * 10 + (sqspec[2] - '0');
        }
        direction = Direction::HORIZONTAL;
    } else if (sqspec.size() == 2) {
        col = sqspec[0] - '0';
        row = sqspec[1] - 'A';
        direction = Direction::VERTICAL;
    } else {
        col = (sqspec[0] - '0') * 10 + (sqspec[1] - '0');
        row = sqspec[2] - 'A';
        direction = Direction::VERTICAL;
    }
    col--;
    if (!(0 <= row && row < Dim)) {
        throw std::runtime_error("invalid ISC square spec");
    }
    if (!(0 <= col && col < Dim)) {
        throw std::runtime_error("invalid ISC square spec");
    }
    return {row, col, direction};
}

std::optional<Move> make_move_isc_notation(const Board& b, std::string sqspec, std::string word, int score) {
    // ISC uses lower case letters for regular, and upper case for blanks
    word = convert_to_internal_word(word);

    auto& board = b.brd;
    auto&& [row, col, direction] = _parse_isc_spec(sqspec);
    Move result;
    result.player = b.n_moves % 2 == 0 ? Player::Player1 : Player::Player2;
    result.score = 0;
    result.square = row * Dim + col;
    result.direction = direction;
    result.length = static_cast<int>(word.size());
    std::fill(std::begin(result.tiles), std::end(result.tiles), Empty);
    std::fill(std::begin(result.squares), std::end(result.squares), InvalidSquare);

    const int start = result.square;
    const int step = static_cast<int>(result.direction);
    const int stop = start + step * Dim;
    if (start + step * result.length >= stop) {
        DEBUG("error: word to long to fit: '%s' starting at %s", word.c_str(), SquareNames[start]);
        return std::nullopt;
    }

    int tiles = 0;
    for (int i = 0; i < result.length; ++i) {
        const int sq = start + i * step;
        assert(start <= sq && sq < stop);
        if (board[sq] == Empty) {
            result.tiles[tiles] = word[i];
            result.squares[tiles] = sq;
            ++tiles;
        } else if (board[sq] != word[i]) {
            DEBUG("error: tried to play letter '%c' on %s that is occupied by %c", word[i], SquareNames[sq], board[sq]);
            return std::nullopt;
        }
    }

    return result;
}

std::optional<Move> make_move(Board& b, const GuiMove& m) noexcept {
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

    bool h8_played = std::find(squares_begin, squares_end, Sq_H8);
    bool adjacent_to_played_square = std::any_of(squares_begin, squares_end, [&board](Square square) -> bool {
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
    });
    if (!h8_played && !adjacent_to_played_square) {
        DEBUG("error: play not adjacent to any played tiles");
        return std::nullopt;
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

#if 0
    {  // TEMP TEMP
        DEBUG("Dumping sorted tiles:");
        for (int i = 0; i < NumTilesRack; ++i) {
            const auto square = squares[i];
            const auto tile = tiles[i];
            DEBUG("tile='%c' square=%s (%d)", tile, SquareNames[square], square);
        }
    }
#endif

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
    //   4) if first move, H8 must be occupied
    //   5) play connects to a previous play if not first move
    //
    // Need to verify that:
    //   1) play is contiguous in one direction
    //
    // Need to determine:
    //   1) starting square
    //   2) direction
    //   3) score

    // if (board[Sq_H8] == Empty) {
    //     DEBUG("error: did not occupy H8 square on first move");
    //     return _undo_move_and_return_null();
    // }

    Move result;
    result.player = b.n_moves % 2 == 0 ? Player::Player1 : Player::Player2;
    result.score = 0;
    result.square = InvalidSquare;
    // result.direction = /* INVALID */;
    result.length = 0;
    result.tiles = std::move(tiles);
    result.squares = std::move(squares);

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
        DEBUG("HORIZONTAL LENGTH: [%d, %d) -> %d", horz_start, horz_stop, result.length);
    }

    if (same_col) {
        const int col_start = first_tile_col;  // inclusive
        const int col_stop = NumSquares;       // exclusive
        int vert_start = first_tile_sq + Up;
        while (vert_start >= col_start && board[vert_start] != Empty) {
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
        DEBUG("VERTICAL LENGTH: [%d, %d) -> %d", vert_start, vert_stop, length);
        if (length >= result.length) {
            result.square = vert_start;
            result.direction = Direction::VERTICAL;
            result.length = length;
        }
    }

    result.score = score_move(b, result);
    clear_multiplier_for_squares(result.squares, b.double_letter_squares_);
    clear_multiplier_for_squares(result.squares, b.triple_letter_squares_);
    b.n_moves++;

    return result;
}
