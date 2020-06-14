#pragma once

#include <iosfwd>
#include <string_view>
#include <optional>
#include <cassert>

namespace hume {

enum class Direction
{
    Horz, // = CICERO_HORZ,
    Vert, // = CICERO_VERT,
};

constexpr bool inrange(char c, char first, char last) noexcept
{ return first <= c && c <= last; }

// TODO: make these safer
constexpr Direction isc_direction(std::string_view sqspec) noexcept
{
    const auto c = sqspec[0];
    return (
        inrange(c, 'A', 'O') || inrange(c, 'a', 'o') ?
        Direction::Horz : Direction::Vert
    );
}

constexpr Direction gcg_direction(std::string_view sqspec) noexcept
{
    const auto c = sqspec[0];
    return (
        inrange(c, 'A', 'O') || inrange(c, 'a', 'o') ?
        Direction::Vert : Direction::Horz
    );
}

class Square
{
    static constexpr int Dim = 15;
    static constexpr int NumSquares = Dim*Dim;

public:
    static constexpr std::optional<Square> make(int sq) noexcept
    // { return valid(sq) ? { Square{sq} } : { std::nullopt }; }
    {
        if (valid(sq)) return Square{sq};
        return std::nullopt;
    }
    static std::optional<Square> from_gcg_name(std::string_view name) noexcept;
    static std::optional<Square> from_isc_name(std::string_view name) noexcept;

    constexpr Square() noexcept : sq{0} {}
    constexpr Square(const Square& other) noexcept : sq{other.sq} {}
    constexpr Square(Square&& other) noexcept : sq{other.sq} { other.sq = -1; }
    constexpr Square& operator=(const Square& other) noexcept {
        sq = other.sq;
        return *this;
    }
    constexpr Square& operator=(Square&& other) noexcept {
        sq = other.sq;
        other.sq = -1;
        return *this;
    }
    constexpr bool ok() const noexcept { return valid(sq); }
    constexpr int value() const noexcept { assert(ok()); return sq; }
    constexpr const char* name() const noexcept
    {
        assert(ok());
#ifndef NDEBUG
        return ok() ? SquareNames[sq] : "invalid";
#else
        return SquareNames[sq];
#endif
    }
    constexpr int column() const noexcept { assert(ok()); return sq % Dim; }
    constexpr int col() const noexcept { return column(); }
    constexpr int row() const noexcept { assert(ok()); return sq / Dim; }

private:
    explicit constexpr Square(int s) noexcept : sq{s} { assert(ok()); }

    static constexpr bool valid(int sq) noexcept
    { return 0 <= sq && sq < NumSquares; }

    friend constexpr bool operator==(Square lhs, Square rhs) noexcept
    { return lhs.sq == rhs.sq; }

    friend constexpr bool operator!=(Square lhs, Square rhs) noexcept
    { return lhs.sq != rhs.sq; }

    friend constexpr bool operator<(Square lhs, Square rhs) noexcept
    { return lhs.sq < rhs.sq; }

    friend constexpr bool operator>(Square lhs, Square rhs) noexcept
    { return lhs.sq > rhs.sq; }

    friend constexpr bool operator<=(Square lhs, Square rhs) noexcept
    { return lhs.sq <= rhs.sq; }

    friend constexpr bool operator>=(Square lhs, Square rhs) noexcept
    { return lhs.sq >= rhs.sq; }

    friend std::ostream& operator<<(std::ostream& os, Square sq) noexcept;

private:
    int sq;

private:
    static constexpr const char* const SquareNames[225] = {
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
};

} // ~hume
