#pragma once

#include <string>
// #include <iostream>
// #include <cicero/cicero.h>

namespace hume {

// TODO: fix me
using Rack = std::string;

// struct Rack {};

// in scrabble.h
// inline std::ostream& operator<<(std::ostream& os, const cicero_rack& rack)
// {
//     for (int i = 0; i < 26; ++i) {
//         for (int j = 0; j < rack.tiles[i]; ++j) {
//             os << ('A' + i);
//         }
//     }
//     constexpr std::size_t BlankTile = 26;
//     for (int j = 0; j < rack.tiles[BlankTile]; ++j) {
//         os << static_cast<char>(CICERO_BLANK_TILE);
//     }
//     return os;
// }

} // ~hume
