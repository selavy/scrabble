#pragma once

#include <array>
#include <cinttypes>
#include <cstdint>

constexpr int NumTilesRack = 7;

using Tile = char;
using Square = int;
using Score = int;
using TileFreq = int;
using Tiles = std::array<Tile, NumTilesRack>;
using Rack = Tiles;

constexpr int Dim = 15;
constexpr int NumSquares = Dim * Dim;
constexpr int NumBlankTiles = 2;
constexpr int MinWordLength = 2;
constexpr int MaxWordLength = Dim;
constexpr int NumTotalTiles = 100;
constexpr int NumTileKinds = 27;
constexpr Tile Empty = ' ';
constexpr Tile Blank = '?';
constexpr Square InvalidSquare = NumSquares;
constexpr int Left = -1;
constexpr int Right = 1;
constexpr int Up = -Dim;
constexpr int Down = Dim;

constexpr Tiles all_empty_tiles = {
    Empty, Empty, Empty, Empty, Empty, Empty, Empty,
};

// clang-format off
enum class Sq : int {
     A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8,  A9, A10, A11, A12, A13, A14, A15,
     B1,  B2,  B3,  B4,  B5,  B6,  B7,  B8,  B9, B10, B11, B12, B13, B14, B15,
     C1,  C2,  C3,  C4,  C5,  C6,  C7,  C8,  C9, C10, C11, C12, C13, C14, C15,
     D1,  D2,  D3,  D4,  D5,  D6,  D7,  D8,  D9, D10, D11, D12, D13, D14, D15,
     E1,  E2,  E3,  E4,  E5,  E6,  E7,  E8,  E9, E10, E11, E12, E13, E14, E15,
     F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9, F10, F11, F12, F13, F14, F15,
     G1,  G2,  G3,  G4,  G5,  G6,  G7,  G8,  G9, G10, G11, G12, G13, G14, G15,
     H1,  H2,  H3,  H4,  H5,  H6,  H7,  H8,  H9, H10, H11, H12, H13, H14, H15,
     I1,  I2,  I3,  I4,  I5,  I6,  I7,  I8,  I9, I10, I11, I12, I13, I14, I15,
     J1,  J2,  J3,  J4,  J5,  J6,  J7,  J8,  J9, J10, J11, J12, J13, J14, J15,
     K1,  K2,  K3,  K4,  K5,  K6,  K7,  K8,  K9, K10, K11, K12, K13, K14, K15,
     L1,  L2,  L3,  L4,  L5,  L6,  L7,  L8,  L9, L10, L11, L12, L13, L14, L15,
     M1,  M2,  M3,  M4,  M5,  M6,  M7,  M8,  M9, M10, M11, M12, M13, M14, M15,
     N1,  N2,  N3,  N4,  N5,  N6,  N7,  N8,  N9, N10, N11, N12, N13, N14, N15,
     O1,  O2,  O3,  O4,  O5,  O6,  O7,  O8,  O9, O10, O11, O12, O13, O14, O15,
};
constexpr std::array<const char* const, 226> SquareNames = {
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
    "INVALID",
};
constexpr std::array<int, 128> letter_values = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  1,  3,  3,  2,  1,  4,  2,  4,  1,  8,  5,  1,  3,  1,  1,
     3, 10,  1,  1,  1,  1,  4,  4,  8,  4, 10,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

constexpr std::array<int, 226> triple_word_squares = {
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//  -------------------------------------------
    3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // G
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, // H
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // I
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // J
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // K
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // L
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // M
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // N
    3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, // O
    1,
};

constexpr std::array<int, 226> double_word_squares = {
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//  -------------------------------------------
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, // B
    1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, // C
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, // D
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // G
    1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, // H
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // I
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // J
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, // K
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, // L
    1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, // M
    1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, // N
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // O
    1,
};

constexpr std::array<int, 226> triple_letter_squares = {
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//  -------------------------------------------
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, // F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // G
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // H
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // I
    1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, // J
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // K
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // L
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // M
    1, 1, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1, // N
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // O
    1,
};

constexpr std::array<int, 226> double_letter_squares = {
//  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//  -------------------------------------------
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, // C
    2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F
    1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, // G
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, // H
    1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, // I
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // J
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // K
    2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, // L
    1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, // M
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // N
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, // O
    1,
};
// clang-format on
