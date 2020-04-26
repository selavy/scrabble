#ifndef CICERO_TABLES_H_
#define CICERO_TABLES_H_


static const int letter_values[128] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  1,  3,  3,  2,  1,  4,  2,  4,  1,  8,  5,  1,  3,  1,  1,
     3, 10,  1,  1,  1,  1,  4,  4,  8,  4, 10,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

// TODO(peter): had 226 entries so that InvalidSquare will return 1, but don't think that I need that anymore
static const int triple_word_squares[226] = {
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

static const int double_word_squares[226] = {
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

static const int triple_letter_squares[226] = {
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

static const int double_letter_squares[226] = {
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

#endif // CICERO_TABLES_H_
