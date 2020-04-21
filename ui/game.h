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
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <map>

#include "tables.h"

#define AsSquare(x) static_cast<int>(x)
#define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__);
#define ERROR(fmt, ...) fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__);

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
constexpr const char* GetDirectionName(Direction x) {
    switch (x) {
        case Direction::HORIZONTAL:
            return "Horizontal";
        case Direction::VERTICAL:
            return "Vertical";
    }
    return "Unknown";
}

enum class Player : int {
    Player1 = 0,
    Player2 = 1,
};

const char* GetPlayerName(Player player) {
    switch (player) {
        case Player::Player1:
            return "Player 1";
        case Player::Player2:
            return "Player 2";
    }
    return "Unknown";
}

constexpr int ix(char row, int col) {
    assert('A' <= row && row <= 'O');
    assert(1 <= col && col <= Dim);
    return (row - 'A') * Dim + (col - 1);
}

// TODO: need to distinguish between a tile where
// blank = Blank and blank = lower case
constexpr bool isBlankTile(Tile t) noexcept {
    return (t == Blank) || ('a' <= t && t <= 'z');
}

// clang-format off
struct IscMove
{
    // format: ((?:\d\d?[A-O])|(?:[A-O]\d\d?)) ([a-zA-Z]+) (\d+)
    std::string sqspec;
    std::string root;
    int         score = -1;
};
// clang-format on

std::ostream& operator<<(std::ostream& os, const IscMove& m) {
    os << m.sqspec << " " << m.root << " " << m.score;
    return os;
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

struct Bag {
    constexpr static std::array<TileFreq, 27> starting_frequencies = {
        9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1, 2,
    };

    Bag() noexcept : Bag(std::random_device{}()) {}

    explicit Bag(int seed) noexcept {
        tiles.reserve(NumTotalTiles);
        for (size_t i = 0; i < starting_frequencies.size(); ++i) {
            char c = i < 26 ? 'A' + i : '?';
            tiles.insert(tiles.end(), starting_frequencies[i], c);
        }
        assert(tiles.size() == NumTotalTiles);
        std::mt19937 gen(seed);
        std::shuffle(tiles.begin(), tiles.end(), gen);
    }

    std::optional<Tile> draw() noexcept {
        if (tiles.empty()) {
            return std::nullopt;
        }
        auto tile = tiles.back();
        tiles.pop_back();
        return tile;
    }

    std::vector<Tile> tiles;
};

void draw_tiles(Bag& bag, Rack& rack) {
    for (;;) {
        auto empty_it = std::find(std::begin(rack), std::end(rack), Empty);
        if (empty_it == std::end(rack)) {
            break;
        }
        auto maybe_tile = bag.draw();
        if (!maybe_tile) {
            break;
        }
        *empty_it = *maybe_tile;
    }
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
    Letters letters = {};  // null terminated c-string format and all caps
    int length = 0;

    constexpr Word() noexcept = default;

    constexpr Word(const char* word) noexcept {
        length = strlen(word);
        assert(MinWordLength <= length && length <= MaxWordLength);
        for (int i = 0; i < length; ++i) {
            assert('A' <= word[i] && word[i] <= 'Z');
            letters[i] = word[i];
        }
        assert(strlen(&letters[0]) == length);
#ifndef NDEBUG
        for (int i = length; i <= MaxWordLength; ++i) {
            assert(letters[i] == '\0');
        }
#endif
    }

    // c_str() isn't constexpr??
    /*constexpr*/ Word(const std::string& word) noexcept : Word(word.c_str()) {}

    constexpr Word(const Word& other) noexcept : letters{other.letters}, length{other.length} {}

    constexpr Word(Word&& other) noexcept : letters{other.letters}, length{other.length} {
        // TODO(peter): temp temp -- don't actually need to clear other.letters
        std::fill(std::begin(other.letters), std::end(other.letters), '\0');
        other.length = 0;
    }

    constexpr Word& operator=(const Word& other) noexcept {
        letters = other.letters;
        length = other.length;
        return *this;
    }

    constexpr Word& operator=(Word&& other) noexcept {
        if (this != &other) {
            *this = other;
            // TODO(peter): temp temp -- don't actually need to clear other.letters
            std::fill(std::begin(other.letters), std::end(other.letters), '\0');
            other.length = 0;
        }
        return *this;
    }

    constexpr int size() const noexcept { return length; }
    constexpr const char* c_str() const noexcept { return &letters[0]; }
    std::string str() const noexcept { return {&letters[0], &letters[length]}; }

    void append(Tile c) {
        assert(length <= MaxWordLength);
        if ('A' <= c && c <= 'Z') {
            letters[length++] = c;
        } else if ('a' <= c && c <= 'z') {
            letters[length++] = static_cast<char>('A' + (c - 'a'));
        } else {
            assert(0 && "invalid tile");
        }
    }

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

    bool operator==(const Word& other) const noexcept {
        return memcmp(&this->letters[0], &other.letters[0], letters.size() * sizeof(char)) == 0;
    }

    bool operator!=(const Word& other) const noexcept { return !(*this == other); }

    bool operator<(const Word& other) const noexcept { return strcmp(this->c_str(), other.c_str()) < 0; }
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

bool operator==(const GuiMove& lhs, const GuiMove& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i].first != rhs[i].first) {
            return false;
        }
        if (lhs[i].second != rhs[i].second) {
            return false;
        }
    }
    return true;
}

bool operator!=(const GuiMove& lhs, const GuiMove& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const GuiMove& move) {
    os << "[ ";
    for (auto&& [tile, square] : move) {
        os << "(" << tile << ", " << SquareNames[square] /* << " [" << square << "]" */ << ") ";
    }
    os << "]";
    return os;
}

const char* const GetSqName(Square sq) {
    if (sq < 0 || sq > SquareNames.size()) {
        return "Invalid [out-of-range]";
    } else {
        return SquareNames[sq];
    }
}

// clang-format off
struct Move
{
    Player            player;
    Score             score;
    Square            square;
    Direction         direction;
    int               length; // total length of play; includes already placed tiles
    Tiles             tiles;
    RackArray<Square> squares;

    // TEMP TEMP
    std::string              root_word;
    std::vector<std::string> words_formed;
};
// clang-format on

std::ostream& operator<<(std::ostream& os, const Tiles& tiles) {
    os << "{ ";
    for (auto tile : tiles) {
        if (tile == Empty) {
            continue;
        }
        os << "'" << tile << "' ";
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const RackArray<Square>& squares) {
    os << "{ ";
    for (auto square : squares) {
        if (square == InvalidSquare) {
            continue;
        }
        os << GetSqName(square) << " ";
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Move& move)
{
    // clang-format off
    os << "Player   : " << (static_cast<int>(move.player) + 1) << "\n"
       << "Score    : " << move.score << "\n"
       << "Square   : " << GetSqName(move.square) << "\n"
       << "Direction: " << GetDirectionName(move.direction) << "\n"
       << "Length   : " << move.length << "\n"
       << "Tiles    : " << move.tiles << "\n"
       << "Squares  : " << move.squares << "\n"
       ;
    return os;
    // clang-format on
}

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
void play_move(Board& b, const Move& m) noexcept {
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

// TODO: I think the max number of words formed on a turn is 15 + 1 = 16
//       can at most form 1 word horizontally of length 15, then potentially
//       could form 15 cross words for a total of 16?
std::vector<Word> find_words(const Board& b, const Move& m) noexcept {
    std::vector<Word> words;
    auto& board = b.brd;

    {  // root word score along `m.direction` direction
        const int start = m.square;
        const int step = static_cast<int>(m.direction);
        const int stop = start + step * m.length;
        Word word;
        for (int sq = start; sq != stop; sq += step) {
            word.append(board[sq]);
        }
        assert(word.size() >= MinWordLength);
        words.emplace_back(std::move(word));
    }

    for (const auto root : m.squares) {
        if (root == InvalidSquare) {
            break;
        }
        const int start = m.direction == Direction::HORIZONTAL ? getcol(root) : Dim * getrow(root);
        const int step = static_cast<int>(flip_direction(m.direction));
        const int stop = start + Dim * step;
        int sq = root - step;
        while (sq >= start && board[sq] != Empty) {
            sq -= step;
        }
        sq += step;
        Word word;
        for (; sq < stop && board[sq] != Empty; sq += step) {
            word.append(board[sq]);
        }
        if (word.size() > 1) {
            words.emplace_back(std::move(word));
        }
    }

    return words;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Word>& words) {
    os << "{ ";
    for (const auto& word : words) {
        os << word << " ";
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& words) {
    os << "{ ";
    for (const auto& word : words) {
        os << word << " ";
    }
    os << "}";
    return os;
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
            m.words_formed.push_back(word); // TEMP TEMP
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
std::tuple<int, int, Direction> _parse_isc_spec(const std::string& sqspec) {
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

bool rack_has_tiles(const Rack& rack, const GuiMove& move) {
    std::map<Tile, TileFreq> rackfreq;
    std::map<Tile, TileFreq> movefreq;
    for (auto tile : rack) {
        rackfreq[tile]++;
    }
    for (auto [tile, square] : move) {
        if (isBlankTile(tile)) {
            tile = Blank;
        }
        movefreq[tile]++;
        if (movefreq[tile] > rackfreq[tile]) {
            ERROR("error: attempt to play '%c', which not enough of in rack (%d)", tile, rackfreq[tile]);
            return false;
        }
    }
    return true;
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
    const int mdim = direction == Direction::HORIZONTAL ? row : col;
    const int stop = start + step * std::min(Dim, mdim + result.length);
    if (start + step * result.length >= stop) {
        ERROR("error: word to long to fit: '%s' starting at %s", word.c_str(), SquareNames[start]);
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
            ERROR("error: tried to play letter '%c' on %s that is occupied by %c", word[i], SquareNames[sq], board[sq]);
            return std::nullopt;
        }
    }

    return result;
}

struct GcgMove
{
    std::string nick;
    std::string rack;
    std::string sq;
    std::string tiles;
    int         score;
    int         total;
};

std::ostream& operator<<(std::ostream& os, const GcgMove& m)
{
    os << m.nick << " "
       << m.rack << " "
       << m.sq << " "
       << m.tiles << " "
       << m.score << " "
       << m.total;
    return os;
}

std::optional<GuiMove> make_gui_move_from_gcg(const Board& b, const GcgMove& gcg)
{
    Direction dir = static_cast<Direction>(-1);
    int row;
    int col;
    // NOTE(peter): GCG notation is backwards from what I used internally! The columns
    //              are labeled with letters;
    if (gcg.sq.size() == 2) {
        if ('A' <= gcg.sq[0] && gcg.sq[0] <= 'O') {
            col = gcg.sq[0] - 'A';
            row = gcg.sq[1] - '1';
            dir = Direction::VERTICAL;
        } else {
            row = gcg.sq[0] - '1';
            col = gcg.sq[1] - 'A';
            dir = Direction::HORIZONTAL;
        }
    } else if (gcg.sq.size() == 3) {
        if ('A' <= gcg.sq[0] && gcg.sq[0] <= 'O') {
            col = gcg.sq[0] - 'A';
            row = 10*(gcg.sq[1] - '0') + (gcg.sq[2] - '0') - 1;
            dir = Direction::VERTICAL;
        } else {
            row = 10*(gcg.sq[0] - '0') + (gcg.sq[1] - '0') - 1;
            col = gcg.sq[2] - 'A';
            dir = Direction::HORIZONTAL;
        }
    } else {
        ERROR("error: invalid square specification: '%s'", gcg.sq.c_str());
        return std::nullopt;
    }
    assert(dir == Direction::HORIZONTAL || dir == Direction::VERTICAL);
    if (!((0 <= row && row < Dim) && (0 <= col && col < Dim))) {
        ERROR("error: invalid square specification (index out of range): '%s'", gcg.sq.c_str());
        return std::nullopt;
    }

    const int length = static_cast<int>(gcg.tiles.size());
    const int root   = row*15 + col;
    const int start  = dir == Direction::HORIZONTAL ? getcol(root) : Dim * getrow(root);
    const int stride = static_cast<int>(dir);
    const int stop   = start + stride * Dim;
    if (length < MinWordLength || length > MaxWordLength) {
        ERROR("error: move length too long: %d", length);
        return std::nullopt;
    }

    GuiMove result;
    for (int i = 0; i < length; ++i) {
        const char tile  = gcg.tiles[i];
        const int square = root + i * stride;
        if ((b.brd[square] != Empty) != (tile == '.')) {
            ERROR("error: tiles[%d]='%c' board[%s]='%c'", i, tile, GetSqName(square), b.brd[square]);
            return std::nullopt;
        }
        if (tile != '.') {
            result.emplace_back(tile, square);
        }
    }
    return result;

}

GuiMove make_gui_move_from_isc(const Board& b, const IscMove& isc) {
    GuiMove result;
    const auto root = convert_to_internal_word(isc.root);
    const auto& board = b.brd;
    const auto [row, col, direction] = _parse_isc_spec(isc.sqspec);
    const int len = std::min(static_cast<int>(root.size()), MaxWordLength);
    const int start = row * Dim + col;
    const int step = static_cast<int>(direction);
    const int mdim = direction == Direction::HORIZONTAL ? col : row;
    assert(mdim + len <= Dim);
    const int stop = start + step * (mdim + len); // TODO: this is wrong
    for (int i = 0; i < len; ++i) {
        const int square = start + i * step;
        assert(start <= square && square < stop);
        if (board[square] == Empty) {
            auto tile = root[i];
            result.emplace_back(tile, square);
        } else {
            assert(board[square] == root[i]);
        }
    }
    return result;
}

GuiMove make_gui_move_from_move(const Move& move)
{
    GuiMove result;
    for (std::size_t i = 0; i < move.squares.size(); ++i) {
        auto tile   = move.tiles[i];
        auto square = move.squares[i];
        if (tile == Empty || square == InvalidSquare) {
            break;
        }
        result.emplace_back(tile, square);
    }
    return result;
}

// precondition: `m` had been successfully applied to make_move() --
// if make_move() didn't return successfully, then don't need to
// call this.
void undo_move(Board& b, const GuiMove& m) noexcept {
    for (auto&& [tile, square] : m) {
        b.brd[square] = Empty;
    }
    b.n_moves--;
}

void unplay_move(Board& b, const Move& m) noexcept {
    auto& board = b.brd;
    for (std::size_t i = 0; i < m.squares.size(); ++i) {
        auto square = m.squares[i];
        auto tile   = m.tiles[i];
        if (square == InvalidSquare) {
            break;
        }
        assert(board[square] == tile);
        board[square] = Empty;
    }
    b.n_moves--;
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
        ERROR("error: invalid number of tiles: %zu", m.size());
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
        ERROR("error: squares are not unique");
        return std::nullopt;
    }

    bool same_row = all_same(squares_begin, squares_end, getrow);
    bool same_col = all_same(squares_begin, squares_end, getcol);
    if (!same_row && !same_col) {
        ERROR("error: squares are not all on the same row or column");
        return std::nullopt;
    }
    assert((same_row || same_col) || (same_row && same_col && m.size() == 1));

    bool h8_played = std::find(squares_begin, squares_end, AsSquare(Sq::H8)) != squares_end;
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
        ERROR("error: play not adjacent to any played tiles h8_player=%s adjacent_to_played_square=%s",
                h8_played?"TRUE":"FALSE", adjacent_to_played_square?"TRUE":"FALSE");
        return std::nullopt;
    }

    for (int i = 0; i < n_tiles; ++i) {
        const auto square = squares[i];
        const auto tile = tiles[i];
        if (board[square] != Empty) {
            for (int j = i - 1; j >= 0; --j) {
                board[square] = Empty;
            }
            ERROR("error: square not available: %s (%d)", SquareNames[square], square);
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
        // DEBUG("horz=[%d, %d) [%s, %s]", horz_start, horz_stop, SquareNames[horz_start], SquareNames[horz_stop + Left]);

        if (last_tile_sq >= horz_stop) {
            ERROR("error: tiles are not contiguous horizontally: last_tile=%d horz_stop=%d", last_tile_sq, horz_stop);
            return _undo_move_and_return_null();
        }

        result.square = horz_start;
        result.direction = Direction::HORIZONTAL;
        result.length = horz_stop - horz_start;
        // DEBUG("HORIZONTAL LENGTH: [%d, %d) -> %d", horz_start, horz_stop, result.length);
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
        // DEBUG("vert=[%d, %d) [%s, %s]", vert_start, vert_stop, SquareNames[vert_start], SquareNames[vert_stop + Up]);

        if (last_tile_sq >= vert_stop) {
            ERROR("error: tiles are not contiguous vertically");
            return _undo_move_and_return_null();
        }

        int length = (vert_stop - vert_start) / Dim;
        // DEBUG("VERTICAL LENGTH: [%d, %d) -> %d", vert_start, vert_stop, length);
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

