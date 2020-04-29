#include "scrabble.h"
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <re2/re2.h>


namespace scrabble {

constexpr const char* IscSqName(Direction dir, int sq) noexcept
{
    return dir == Direction::Horz ? HorizontalSquareNames[sq] : VerticalSquareNames[sq];
}

std::string InvertWordCase(std::string word)
{
    for (size_t i = 0; i < word.size(); ++i) {
        const char c = word[i];
        if ('a' <= c && c <= 'z') {
            word[i] = static_cast<char>((c - 'a') + 'A');
        } else if ('A' <= c && c <= 'Z') {
            word[i] = static_cast<char>((c - 'A') + 'a');
        } else {
            throw std::runtime_error("invalid word");
        }
    }
    return word;
}

re2::RE2 isc_regex_(R"(\s*((?:\d{1,2}[A-O])|(?:[A-O]\d{1,2}))\s+([a-zA-Z]{2,15})(?:\s+(\d+))?\s*)");

std::optional<Square> Square::from_isc(std::string_view sqspec) noexcept
{
    if (sqspec.size() < 2) {
        return std::nullopt;
    }
    const Direction dir = ('A' <= sqspec[0] && sqspec[0] <= 'O') ? Direction::Horz : Direction::Vert;
    int row = 0;
    int col = 0;
    std::size_t col1 = dir == Direction::Horz ? 1 : 0;
    std::size_t col2 = col1 + 1;
    col = sqspec[col1] - '0';
    if ('0' <= sqspec[col2] && sqspec[col2] <= '9') {
        col *= 10;
        col += sqspec[col2] - '0';
    } else {
        col2 = col1;
    }
    col--;
    row = dir == Direction::Horz ? (sqspec[0] - 'A') : (sqspec[col2+1] - 'A');
    if (!(0 <= row && row < 15) || !(0 <= col && col < 15)) {
        return std::nullopt;
    }
    return Square{row*Dim + col};
}

Move Move::from_isc_spec(const std::string& spec)
{
    assert(isc_regex_.ok());

    std::string sqspec;
    std::string root;
    int score = -1;
    re2::RE2::FullMatch(spec.c_str(), isc_regex_, &sqspec, &root, &score);
    if (sqspec.empty() || root.empty()) {
        throw std::runtime_error("invalid ISC move");
    }
    assert(2 <= sqspec.size() && sqspec.size() <= 3);
    assert(2 <= root.size() && root.size() <= 15);

    const Direction dir = ('A' <= sqspec[0] && sqspec[0] <= 'O') ? Direction::Horz : Direction::Vert;
    const auto maybe_square = Square::from_isc(sqspec);
    if (!maybe_square) {
        throw std::runtime_error("invalid square spec");
    }

    Move result;
    result.direction = dir;
    result.square = *maybe_square;
    result.word = InvertWordCase(std::move(root));
    result.score = score;
    return result;
}

std::ostream& operator<<(std::ostream& os, const Move& move)
{
    os << "\"" << IscSqName(move.direction, move.square.value()) << " " << InvertWordCase(move.word);
    if (move.score >= 0) {
        os << " " << move.score;
    }
    os << "\"";
    return os;
}

bool operator==(const Move& lhs, const Move& rhs) noexcept
{
    return (
            lhs.direction == rhs.direction &&
            lhs.square    == rhs.square    &&
            lhs.word      == rhs.word      &&
            lhs.score     == rhs.score
    );
}

bool operator!=(const Move& lhs, const Move& rhs) noexcept
{
    return !(lhs == rhs);
}

bool operator< (const Move& lhs, const Move& rhs) noexcept
{
    const auto comp = lhs.word.compare(rhs.word);
    if (comp == 0) {
        if (lhs.direction == rhs.direction) {
            return lhs.square < rhs.square;
        }
        return static_cast<int>(lhs.direction) < static_cast<int>(rhs.direction);
    }
    return comp < 0;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Move>& moves)
{
    os << "[ ";
    for (const auto& move : moves) {
        os << move << " ";
    }
    os << "]";
    return os;
}

bool operator==(const std::vector<Move>& lhs, const std::vector<Move>& rhs) noexcept
{
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const std::vector<Move>& lhs, const std::vector<Move>& rhs) noexcept
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const cicero_rack& rack)
{
    os << "\"";
    int num_tiles = 0;
    for (int i = 0; i < 26; ++i) {
        for (int j = 0; j < rack.tiles[i]; ++j) {
            os << static_cast<char>(i + 'A');
            ++num_tiles;
        }
    }
    for (int i = 0; i < rack.tiles[26]; ++i) {
        os << "?";
        ++num_tiles;
    }
    for (; num_tiles++ < 7; ++num_tiles) {
        os << " ";
    }
    os << "\"";
    return os;
}


EngineMove EngineMove::make(cicero* engine, const scrabble::Move& move)
{
    EngineMove result;
    const int step   = static_cast<int>(move.direction);
    const int square = move.square.value();
    int i = 0;
    for (auto c : move.word) {
        const int sq = square + i++ * step;
        const char t = cicero_tile_on_square(engine, sq);
        if (t == CICERO_EMPTY_TILE) {
            result.tiles.push_back(c);
            result.squares.push_back(sq);
        }
    }
    result.move.tiles = &result.tiles[0];
    result.move.squares = &result.squares[0];
    result.move.ntiles = static_cast<int>(result.tiles.size());
    result.move.direction = static_cast<cicero_direction>(move.direction);
    return result;
}

} // ~scrabble
