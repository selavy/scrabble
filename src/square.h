#pragma once

#include <iostream>

namespace hume {

struct Square
{
    constexpr Square() noexcept : sq{-1} {}
    constexpr explicit Square(int s) noexcept : sq{s} {}
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
    constexpr int value() const noexcept { return sq; }

    constexpr bool operator==(Square rhs) const noexcept
    { return this->sq == rhs.sq; }
    constexpr bool operator!=(Square rhs) const noexcept
    { return this->sq != rhs.sq; }
    constexpr bool operator<(Square rhs) const noexcept
    { return this->sq < rhs.sq; }
    constexpr bool operator>(Square rhs) const noexcept
    { return this->sq > rhs.sq; }
    constexpr bool operator<=(Square rhs) const noexcept
    { return this->sq <= rhs.sq; }
    constexpr bool operator>=(Square rhs) const noexcept
    { return this->sq >= rhs.sq; }

private:
    int sq;
};

// std::ostream& operator<<(std::ostream& os, Square sq) noexcept;

} // ~hume
