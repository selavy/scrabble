#pragma once

#include <array>
#include <cstdint>
#include <cinttypes>

constexpr int NumTilesRack = 7;

using Tile = char;
using Square = int;
using Score = int;
using Tiles = std::array<Tile, NumTilesRack>; // MaxWordLength>;

constexpr int Dim = 15;
constexpr int NumSquares = Dim * Dim;
constexpr int NumBlankTiles = 2;
constexpr int MinWordLength = 2;
constexpr int MaxWordLength = Dim;
constexpr Tile Empty = ' ';
constexpr Tile Blank = '?';
constexpr Square InvalidSquare = NumSquares;
constexpr int Left = -1;
constexpr int Right = 1;
constexpr int Up = -Dim;
constexpr int Down = Dim;

constexpr int Sq_A1 = 0;
constexpr int Sq_A2 = 1;
constexpr int Sq_A3 = 2;
constexpr int Sq_A4 = 3;
constexpr int Sq_A5 = 4;
constexpr int Sq_A6 = 5;
constexpr int Sq_A7 = 6;
constexpr int Sq_A8 = 7;
constexpr int Sq_A9 = 8;
constexpr int Sq_A10 = 9;
constexpr int Sq_A11 = 10;
constexpr int Sq_A12 = 11;
constexpr int Sq_A13 = 12;
constexpr int Sq_A14 = 13;
constexpr int Sq_A15 = 14;
constexpr int Sq_B1 = 15;
constexpr int Sq_B2 = 16;
constexpr int Sq_B3 = 17;
constexpr int Sq_B4 = 18;
constexpr int Sq_B5 = 19;
constexpr int Sq_B6 = 20;
constexpr int Sq_B7 = 21;
constexpr int Sq_B8 = 22;
constexpr int Sq_B9 = 23;
constexpr int Sq_B10 = 24;
constexpr int Sq_B11 = 25;
constexpr int Sq_B12 = 26;
constexpr int Sq_B13 = 27;
constexpr int Sq_B14 = 28;
constexpr int Sq_B15 = 29;
constexpr int Sq_C1 = 30;
constexpr int Sq_C2 = 31;
constexpr int Sq_C3 = 32;
constexpr int Sq_C4 = 33;
constexpr int Sq_C5 = 34;
constexpr int Sq_C6 = 35;
constexpr int Sq_C7 = 36;
constexpr int Sq_C8 = 37;
constexpr int Sq_C9 = 38;
constexpr int Sq_C10 = 39;
constexpr int Sq_C11 = 40;
constexpr int Sq_C12 = 41;
constexpr int Sq_C13 = 42;
constexpr int Sq_C14 = 43;
constexpr int Sq_C15 = 44;
constexpr int Sq_D1 = 45;
constexpr int Sq_D2 = 46;
constexpr int Sq_D3 = 47;
constexpr int Sq_D4 = 48;
constexpr int Sq_D5 = 49;
constexpr int Sq_D6 = 50;
constexpr int Sq_D7 = 51;
constexpr int Sq_D8 = 52;
constexpr int Sq_D9 = 53;
constexpr int Sq_D10 = 54;
constexpr int Sq_D11 = 55;
constexpr int Sq_D12 = 56;
constexpr int Sq_D13 = 57;
constexpr int Sq_D14 = 58;
constexpr int Sq_D15 = 59;
constexpr int Sq_E1 = 60;
constexpr int Sq_E2 = 61;
constexpr int Sq_E3 = 62;
constexpr int Sq_E4 = 63;
constexpr int Sq_E5 = 64;
constexpr int Sq_E6 = 65;
constexpr int Sq_E7 = 66;
constexpr int Sq_E8 = 67;
constexpr int Sq_E9 = 68;
constexpr int Sq_E10 = 69;
constexpr int Sq_E11 = 70;
constexpr int Sq_E12 = 71;
constexpr int Sq_E13 = 72;
constexpr int Sq_E14 = 73;
constexpr int Sq_E15 = 74;
constexpr int Sq_F1 = 75;
constexpr int Sq_F2 = 76;
constexpr int Sq_F3 = 77;
constexpr int Sq_F4 = 78;
constexpr int Sq_F5 = 79;
constexpr int Sq_F6 = 80;
constexpr int Sq_F7 = 81;
constexpr int Sq_F8 = 82;
constexpr int Sq_F9 = 83;
constexpr int Sq_F10 = 84;
constexpr int Sq_F11 = 85;
constexpr int Sq_F12 = 86;
constexpr int Sq_F13 = 87;
constexpr int Sq_F14 = 88;
constexpr int Sq_F15 = 89;
constexpr int Sq_G1 = 90;
constexpr int Sq_G2 = 91;
constexpr int Sq_G3 = 92;
constexpr int Sq_G4 = 93;
constexpr int Sq_G5 = 94;
constexpr int Sq_G6 = 95;
constexpr int Sq_G7 = 96;
constexpr int Sq_G8 = 97;
constexpr int Sq_G9 = 98;
constexpr int Sq_G10 = 99;
constexpr int Sq_G11 = 100;
constexpr int Sq_G12 = 101;
constexpr int Sq_G13 = 102;
constexpr int Sq_G14 = 103;
constexpr int Sq_G15 = 104;
constexpr int Sq_H1 = 105;
constexpr int Sq_H2 = 106;
constexpr int Sq_H3 = 107;
constexpr int Sq_H4 = 108;
constexpr int Sq_H5 = 109;
constexpr int Sq_H6 = 110;
constexpr int Sq_H7 = 111;
constexpr int Sq_H8 = 112;
constexpr int Sq_H9 = 113;
constexpr int Sq_H10 = 114;
constexpr int Sq_H11 = 115;
constexpr int Sq_H12 = 116;
constexpr int Sq_H13 = 117;
constexpr int Sq_H14 = 118;
constexpr int Sq_H15 = 119;
constexpr int Sq_I1 = 120;
constexpr int Sq_I2 = 121;
constexpr int Sq_I3 = 122;
constexpr int Sq_I4 = 123;
constexpr int Sq_I5 = 124;
constexpr int Sq_I6 = 125;
constexpr int Sq_I7 = 126;
constexpr int Sq_I8 = 127;
constexpr int Sq_I9 = 128;
constexpr int Sq_I10 = 129;
constexpr int Sq_I11 = 130;
constexpr int Sq_I12 = 131;
constexpr int Sq_I13 = 132;
constexpr int Sq_I14 = 133;
constexpr int Sq_I15 = 134;
constexpr int Sq_J1 = 135;
constexpr int Sq_J2 = 136;
constexpr int Sq_J3 = 137;
constexpr int Sq_J4 = 138;
constexpr int Sq_J5 = 139;
constexpr int Sq_J6 = 140;
constexpr int Sq_J7 = 141;
constexpr int Sq_J8 = 142;
constexpr int Sq_J9 = 143;
constexpr int Sq_J10 = 144;
constexpr int Sq_J11 = 145;
constexpr int Sq_J12 = 146;
constexpr int Sq_J13 = 147;
constexpr int Sq_J14 = 148;
constexpr int Sq_J15 = 149;
constexpr int Sq_K1 = 150;
constexpr int Sq_K2 = 151;
constexpr int Sq_K3 = 152;
constexpr int Sq_K4 = 153;
constexpr int Sq_K5 = 154;
constexpr int Sq_K6 = 155;
constexpr int Sq_K7 = 156;
constexpr int Sq_K8 = 157;
constexpr int Sq_K9 = 158;
constexpr int Sq_K10 = 159;
constexpr int Sq_K11 = 160;
constexpr int Sq_K12 = 161;
constexpr int Sq_K13 = 162;
constexpr int Sq_K14 = 163;
constexpr int Sq_K15 = 164;
constexpr int Sq_L1 = 165;
constexpr int Sq_L2 = 166;
constexpr int Sq_L3 = 167;
constexpr int Sq_L4 = 168;
constexpr int Sq_L5 = 169;
constexpr int Sq_L6 = 170;
constexpr int Sq_L7 = 171;
constexpr int Sq_L8 = 172;
constexpr int Sq_L9 = 173;
constexpr int Sq_L10 = 174;
constexpr int Sq_L11 = 175;
constexpr int Sq_L12 = 176;
constexpr int Sq_L13 = 177;
constexpr int Sq_L14 = 178;
constexpr int Sq_L15 = 179;
constexpr int Sq_M1 = 180;
constexpr int Sq_M2 = 181;
constexpr int Sq_M3 = 182;
constexpr int Sq_M4 = 183;
constexpr int Sq_M5 = 184;
constexpr int Sq_M6 = 185;
constexpr int Sq_M7 = 186;
constexpr int Sq_M8 = 187;
constexpr int Sq_M9 = 188;
constexpr int Sq_M10 = 189;
constexpr int Sq_M11 = 190;
constexpr int Sq_M12 = 191;
constexpr int Sq_M13 = 192;
constexpr int Sq_M14 = 193;
constexpr int Sq_M15 = 194;
constexpr int Sq_N1 = 195;
constexpr int Sq_N2 = 196;
constexpr int Sq_N3 = 197;
constexpr int Sq_N4 = 198;
constexpr int Sq_N5 = 199;
constexpr int Sq_N6 = 200;
constexpr int Sq_N7 = 201;
constexpr int Sq_N8 = 202;
constexpr int Sq_N9 = 203;
constexpr int Sq_N10 = 204;
constexpr int Sq_N11 = 205;
constexpr int Sq_N12 = 206;
constexpr int Sq_N13 = 207;
constexpr int Sq_N14 = 208;
constexpr int Sq_N15 = 209;
constexpr int Sq_O1 = 210;
constexpr int Sq_O2 = 211;
constexpr int Sq_O3 = 212;
constexpr int Sq_O4 = 213;
constexpr int Sq_O5 = 214;
constexpr int Sq_O6 = 215;
constexpr int Sq_O7 = 216;
constexpr int Sq_O8 = 217;
constexpr int Sq_O9 = 218;
constexpr int Sq_O10 = 219;
constexpr int Sq_O11 = 220;
constexpr int Sq_O12 = 221;
constexpr int Sq_O13 = 222;
constexpr int Sq_O14 = 223;
constexpr int Sq_O15 = 224;

// clang-format off
constexpr const char* const SquareNames[225+1] = {
    " A1", " B1", " C1", " D1", " E1", " F1", " G1", " H1", " I1", " J1", " K1", " L1", " M1", " N1", " O1",
	" A2", " B2", " C2", " D2", " E2", " F2", " G2", " H2", " I2", " J2", " K2", " L2", " M2", " N2", " O2",
	" A3", " B3", " C3", " D3", " E3", " F3", " G3", " H3", " I3", " J3", " K3", " L3", " M3", " N3", " O3",
	" A4", " B4", " C4", " D4", " E4", " F4", " G4", " H4", " I4", " J4", " K4", " L4", " M4", " N4", " O4",
	" A5", " B5", " C5", " D5", " E5", " F5", " G5", " H5", " I5", " J5", " K5", " L5", " M5", " N5", " O5",
	" A6", " B6", " C6", " D6", " E6", " F6", " G6", " H6", " I6", " J6", " K6", " L6", " M6", " N6", " O6",
	" A7", " B7", " C7", " D7", " E7", " F7", " G7", " H7", " I7", " J7", " K7", " L7", " M7", " N7", " O7",
	" A8", " B8", " C8", " D8", " E8", " F8", " G8", " H8", " I8", " J8", " K8", " L8", " M8", " N8", " O8",
	" A9", " B9", " C9", " D9", " E9", " F9", " G9", " H9", " I9", " J9", " K9", " L9", " M9", " N9", " O9",
	"A10", "B10", "C10", "D10", "E10", "F10", "G10", "H10", "I10", "J10", "K10", "L10", "M10", "N10", "O10",
	"A11", "B11", "C11", "D11", "E11", "F11", "G11", "H11", "I11", "J11", "K11", "L11", "M11", "N11", "O11",
    "A12", "B12", "C12", "D12", "E12", "F12", "G12", "H12", "I12", "J12", "K12", "L12", "M12", "N12", "O12",
    "A13", "B13", "C13", "D13", "E13", "F13", "G13", "H13", "I13", "J13", "K13", "L13", "M13", "N13", "O13",
    "A14", "B14", "C14", "D14", "E14", "F14", "G14", "H14", "I14", "J14", "K14", "L14", "M14", "N14", "O14",
    "A15", "B15", "C15", "D15", "E15", "F15", "G15", "H15", "I15", "J15", "K15", "L15", "M15", "N15", "O15",
    "Invalid",
};
// clang-format on

// clang-format off
constexpr Tiles all_empty_tiles = {
    // Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
    Empty, Empty, Empty, Empty, Empty, Empty, Empty,
};
// clang-format on
