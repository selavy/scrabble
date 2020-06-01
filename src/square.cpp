#include "square.h"

#include <iostream>

namespace hume {

std::ostream& operator<<(std::ostream& os, Square sq) noexcept
{
    os << sq.name();
    return os;
}

std::optional<Square> Square::from_gcg_name(std::string_view name) noexcept
{
    auto maybe_square = Square::from_isc_name(name);
    if (!maybe_square) {
        return std::nullopt;
    }
    auto square = *maybe_square;
    auto row = square.col();
    auto col = square.row();
    return Square{row*Dim + col};
}

std::optional<Square> Square::from_isc_name(std::string_view name) noexcept
{
    if (name.size() < 2) {
        return std::nullopt;
    }
    const auto dir = isc_direction(name);
    int row = 0;
    int col = 0;
    std::size_t col1 = dir == Direction::Horz ? 1 : 0;
    std::size_t col2 = col1 + 1;
    col = name[col1] - '0';
    if ('0' <= name[col2] && name[col2] <= '9') {
        col *= 10;
        col += name[col2] - '0';
    } else {
        col2 = col1;
    }
    col--;
    std::size_t idx = dir == Direction::Horz ? 0 : col2 + 1;
    if (inrange(name[idx], 'A', 'O')) {
        row = name[idx] - 'A';
    } else if (inrange(name[idx], 'a', 'o')) {
        row = name[idx] - 'a';
    } else {
        return std::nullopt;
    }
    if (!(0 <= row && row < 15) || !(0 <= col && col < 15)) {
        return std::nullopt;
    }
    return Square{row*Dim + col};
}

} // ~hume
