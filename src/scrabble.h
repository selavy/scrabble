#pragma once

#include <cassert>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <vector>
#include <iosfwd>
#include <string_view>
#include <optional>
#include <cicero/cicero.h>


// general utilities that might useful

constexpr const char* const SquareNames[225] = {
    " A1", " A2", " A3", " A4", " A5", " A6", " A7", " A8", " A9", "A10", "A11", "A12", "A13", "A14", "A15",
    " B1", " B2", " B3", " B4", " B5", " B6", " B7", " B8", " B9", "B10", "B11", "B12", "B13", "B14", "B15",
    " C1", " C2", " C3", " C4", " C5", " C6", " C7", " C8", " C9", "C10", "C11", "C12", "C13", "C14", "C15",
    " D1", " D2", " D3", " D4", " D5", " D6", " D7", " D8", " D9", "D10", "D11", "D12", "D13", "D14", "D15",
    " E1", " E2", " E3", " E4", " E5", " E6", " E7", " E8", " E9", "E10", "E11", "E12", "E13", "E14", "E15",
    " F1", " F2", " F3", " F4", " F5", " F6", " F7", " F8", " F9", "F10", "F11", "F12", "F13", "F14", "F15",
    " G1", " G2", " G3", " G4", " G5", " G6", " G7", " G8", " G9", "G10", "G11", "G12", "G13", "G14", "G15",
    " H1", " H2", " H3", " H4", " H5", " H6", " H7", " H8", " H9", "H10", "H11", "H12", "H13", "H14", "H15",
    " I1", " I2", " I3", " I4", " I5", " I6", " I7", " I8", " I9", "I10", "I11", "I12", "I13", "I14", "I15",
    " J1", " J2", " J3", " J4", " J5", " J6", " J7", " J8", " J9", "J10", "J11", "J12", "J13", "J14", "J15",
    " K1", " K2", " K3", " K4", " K5", " K6", " K7", " K8", " K9", "K10", "K11", "K12", "K13", "K14", "K15",
    " L1", " L2", " L3", " L4", " L5", " L6", " L7", " L8", " L9", "L10", "L11", "L12", "L13", "L14", "L15",
    " M1", " M2", " M3", " M4", " M5", " M6", " M7", " M8", " M9", "M10", "M11", "M12", "M13", "M14", "M15",
    " N1", " N2", " N3", " N4", " N5", " N6", " N7", " N8", " N9", "N10", "N11", "N12", "N13", "N14", "N15",
    " O1", " O2", " O3", " O4", " O5", " O6", " O7", " O8", " O9", "O10", "O11", "O12", "O13", "O14", "O15",
};

constexpr const char* const HorizontalSquareNames[225] = {
    "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12", "A13", "A14", "A15",
    "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12", "B13", "B14", "B15",
    "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13", "C14", "C15",
    "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15",
    "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "E10", "E11", "E12", "E13", "E14", "E15",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15",
    "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11", "G12", "G13", "G14", "G15",
    "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9", "H10", "H11", "H12", "H13", "H14", "H15",
    "I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8", "I9", "I10", "I11", "I12", "I13", "I14", "I15",
    "J1", "J2", "J3", "J4", "J5", "J6", "J7", "J8", "J9", "J10", "J11", "J12", "J13", "J14", "J15",
    "K1", "K2", "K3", "K4", "K5", "K6", "K7", "K8", "K9", "K10", "K11", "K12", "K13", "K14", "K15",
    "L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8", "L9", "L10", "L11", "L12", "L13", "L14", "L15",
    "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9", "M10", "M11", "M12", "M13", "M14", "M15",
    "N1", "N2", "N3", "N4", "N5", "N6", "N7", "N8", "N9", "N10", "N11", "N12", "N13", "N14", "N15",
    "O1", "O2", "O3", "O4", "O5", "O6", "O7", "O8", "O9", "O10", "O11", "O12", "O13", "O14", "O15",
};

constexpr const char* const VerticalSquareNames[225] = {
    "1A", "2A", "3A", "4A", "5A", "6A", "7A", "8A", "9A", "10A", "11A", "12A", "13A", "14A", "15A",
    "1B", "2B", "3B", "4B", "5B", "6B", "7B", "8B", "9B", "10B", "11B", "12B", "13B", "14B", "15B",
    "1C", "2C", "3C", "4C", "5C", "6C", "7C", "8C", "9C", "10C", "11C", "12C", "13C", "14C", "15C",
    "1D", "2D", "3D", "4D", "5D", "6D", "7D", "8D", "9D", "10D", "11D", "12D", "13D", "14D", "15D",
    "1E", "2E", "3E", "4E", "5E", "6E", "7E", "8E", "9E", "10E", "11E", "12E", "13E", "14E", "15E",
    "1F", "2F", "3F", "4F", "5F", "6F", "7F", "8F", "9F", "10F", "11F", "12F", "13F", "14F", "15F",
    "1G", "2G", "3G", "4G", "5G", "6G", "7G", "8G", "9G", "10G", "11G", "12G", "13G", "14G", "15G",
    "1H", "2H", "3H", "4H", "5H", "6H", "7H", "8H", "9H", "10H", "11H", "12H", "13H", "14H", "15H",
    "1I", "2I", "3I", "4I", "5I", "6I", "7I", "8I", "9I", "10I", "11I", "12I", "13I", "14I", "15I",
    "1J", "2J", "3J", "4J", "5J", "6J", "7J", "8J", "9J", "10J", "11J", "12J", "13J", "14J", "15J",
    "1K", "2K", "3K", "4K", "5K", "6K", "7K", "8K", "9K", "10K", "11K", "12K", "13K", "14K", "15K",
    "1L", "2L", "3L", "4L", "5L", "6L", "7L", "8L", "9L", "10L", "11L", "12L", "13L", "14L", "15L",
    "1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M",
    "1N", "2N", "3N", "4N", "5N", "6N", "7N", "8N", "9N", "10N", "11N", "12N", "13N", "14N", "15N",
    "1O", "2O", "3O", "4O", "5O", "6O", "7O", "8O", "9O", "10O", "11O", "12O", "13O", "14O", "15O",
};

constexpr int Dim = 15;
constexpr int NumSquares = Dim*Dim;

constexpr const char* SQ(int sq) noexcept
{
    assert(0 <= sq && sq < NumSquares);
    return SquareNames[sq];
}

constexpr int SqCol(int sq) noexcept { return sq % Dim; }

constexpr int SqRow(int sq) noexcept { return sq / Dim; }

constexpr const char* IscSQ(int dir, int sq)
{
    assert(0 <= sq && sq < NumSquares);
    assert(dir == 1 || dir == Dim);
    return dir == 1 ? HorizontalSquareNames[sq] : VerticalSquareNames[sq];
}

struct SqDir {
    constexpr SqDir(int s, int d) noexcept : sq{s}, dir{d} {}
    int sq; int dir;
};

constexpr SqDir SqNumDir(const char *const name)
{
    for (int i = 0; i < 225; ++i) {
        if (strcmp(name, HorizontalSquareNames[i]) == 0) {
            return SqDir(i, 1);
        }
    }
    for (int i = 0; i < 225; ++i) {
        if (strcmp(name, VerticalSquareNames[i]) == 0) {
            return SqDir(i, Dim);
        }
    }
    // assert(0 && "invalid square name");
    return SqDir(-1, -1);
}

constexpr int IX(const char *const name) noexcept
{
    int i = 0;
    for (auto* sq : HorizontalSquareNames) {
        if (strcmp(name, sq) == 0) {
            return i;
        }
        ++i;
    }
    i = 0;
    for (auto* sq : VerticalSquareNames) {
        if (strcmp(name, sq) == 0) {
            return i;
        }
        ++i;
    }
    // throw std::runtime_error("invalid square");
    return -1;
}

namespace scrabble {

enum class Direction
{
    Horz = CICERO_HORZ,
    Vert = CICERO_VERT,
};

constexpr bool inrange(char c, char first, char last) noexcept { return first <= c && c <= last; }

// TODO: make these safer
constexpr Direction isc_direction(std::string_view sqspec) noexcept {
    const auto c = sqspec[0];
    return inrange(c, 'A', 'O') || inrange(c, 'a', 'o') ? Direction::Horz : Direction::Vert;
}

constexpr Direction gcg_direction(std::string_view sqspec) noexcept {
    const auto c = sqspec[0];
    return inrange(c, 'A', 'O') || inrange(c, 'a', 'o') ? Direction::Vert : Direction::Horz;
}

struct Square
{
    constexpr Square() noexcept : val{-1} {}
    constexpr explicit Square(int square) noexcept : val{square} { assert(valid()); }
    constexpr Square(const Square& other) noexcept : val{other.val} {}
    constexpr Square(Square&& other) noexcept : val{other.val} { other.val = -1; }
    constexpr Square& operator=(const Square& other) noexcept {
        val = other.val;
        return *this;
    }
    constexpr Square& operator=(Square&& other) noexcept {
        val = other.val;
        other.val = -1;
        return *this;
    }
    constexpr bool valid() const noexcept { return 0 <= val && val < 225; }
    constexpr int row() const noexcept { return val / Dim; }
    constexpr int col() const noexcept { return val % Dim; }
    constexpr const char* name() const noexcept {
        if (valid()) {
            return SquareNames[val];
        } else {
            return "InvalidSquare";
        }
    }
    static std::optional<Square> from_isc(std::string_view spec) noexcept;
    static std::optional<Square> from_gcg(std::string_view spec) noexcept;
    explicit operator int() const noexcept { return val; }
    constexpr int value() const noexcept { return val; }
    int val;
};

constexpr bool operator==(Square lhs, Square rhs) noexcept
{
    return lhs.val == rhs.val;
}

constexpr bool operator!=(Square lhs, Square rhs) noexcept
{
    return !(lhs.val == rhs.val);
}

constexpr bool operator<(Square lhs, Square rhs) noexcept
{
    return lhs.val < rhs.val;
}

struct Move
{
    Square      square;
    Direction   direction;
    std::string word;       // upper case is regular tile, lower case is blank
    int         score;      // -1 if unset

    // e.g. "8G flip 18", "I7 kiva 20"
    static Move from_isc_spec(const std::string& spec);
};

struct EngineMove
{
    std::vector<char> tiles;
    std::vector<int>  squares;
    cicero_move       move;

    static EngineMove make(cicero* engine, const scrabble::Move& move);
};

std::ostream& operator<<(std::ostream& os, const Move& move);
bool operator==(const Move& lhs, const Move& rhs) noexcept;
bool operator!=(const Move& lhs, const Move& rhs) noexcept;
bool operator< (const Move& lhs, const Move& rhs) noexcept;
std::ostream& operator<<(std::ostream& os, const std::vector<Move>& moves);
bool operator==(const std::vector<Move>& lhs, const std::vector<Move>& rhs) noexcept;
bool operator!=(const std::vector<Move>& lhs, const std::vector<Move>& rhs) noexcept;
std::ostream& operator<<(std::ostream& os, const cicero_rack& rack);

} // scrabble

constexpr scrabble::Square IXSQ(const char *const name) noexcept
{
    return scrabble::Square{IX(name)};
}
