#!/usr/bin/env python


N_COLS = 15
N_ROWS = N_COLS
N_SQUARES = N_COLS * N_ROWS
A = 0
B = 1
C = 2
D = 3
E = 4
F = 5
G = 6
H = 7
I = 8
J = 9
K = 10
L = 11
M = 12
N = 13
O = 14

triple_words = (
    (A,  1),
    (A,  8),
    (A, 15),
    (H,  1),
    (H, 15),
    (O,  1),
    (O,  8),
    (O, 15),
)
assert len(triple_words) == 8

double_words = [
    (B, 2),
    (C, 3),
    (D, 4),
    (E, 5),

    (E, 11),
    (D, 12),
    (C, 13),
    (B, 14),

    (H, 8),

    (K, 5),
    (L, 4),
    (M, 3),
    (N, 2),

    (K, 11),
    (L, 12),
    (M, 13),
    (N, 14),
]
assert len(double_words) == 17

double_letters = [
    (A, 4),
    (A, 12),
    (C, 7),
    (C, 9),
    (D, 1),
    (D, 8),
    (D, 15),
    (G, 3),
    (G, 7),
    (G, 9),
    (G, 13),
    (H, 4),
    (H, 12),
    (I, 3),
    (I, 7),
    (I, 9),
    (I, 13),
    (L, 1),
    (L, 8),
    (L, 15),
    (M, 7),
    (M, 9),
    (O, 4),
    (O, 13),
]
assert len(double_letters) == 24

triple_letters = [
    (B, 6),
    (B, 10),
    (F, 2),
    (F, 6),
    (F, 10),
    (F, 14),
    (J, 2),
    (J, 6),
    (J, 10),
    (J, 14),
    (N, 6),
    (N, 10),
]
assert len(triple_letters) == 12


def ix(y, x):
    assert A <= y <= O, f"invalid row: {y}"
    assert 1 <= x <= 15, f"invalid col: {x}"
    return y*N_COLS + (x-1)


def print_board(board):
    print("     1   2   3   4   5   6   7   8   9   0   1   2   3   4   5  ")
    print("   -------------------------------------------------------------")
    for row in range(N_ROWS):
        rr = []
        for col in range(N_COLS):
            rr.append(board[row*N_COLS + col])
        row_name = chr(ord('A') + row)
        print(f"{row_name}  | {' | '.join(rr)} |")
        print("   -------------------------------------------------------------")


tw_board = [' ']*N_SQUARES
for row, col in triple_words:
    tw_board[ix(row, col)] = '3'

dw_board = [' ']*N_SQUARES
for row, col in double_words:
    dw_board[ix(row, col)] = '2'

dl_board = [' ']*N_SQUARES
for row, col in double_letters:
    dl_board[ix(row, col)] = '2'

tl_board = [' ']*N_SQUARES
for row, col in triple_letters:
    tl_board[ix(row, col)] = '3'


def xlate_board(board):
    return [1 if x == ' ' else int(x) for x in board]


if 1:
    print_board(tw_board)

if 0:
    print_board(dw_board)

if 0:
    print_board(dl_board)

if 0:
    print_board(tl_board)

def print_array_impl(name, dtype, vals, specifier, n, const_specifier=''):
    const_specifier = const_specifier.strip()
    if const_specifier:
        const_specifier += ' '
    print(f"{const_specifier}{dtype} {name}[{len(vals)}] = {{")
    groups = []
    grp = []
    for v in vals:
        if len(grp) >= n:
            groups.append(grp)
            grp = [v]
        else:
            grp.append(v)
    if grp:
        groups.append(grp)
    for grp in groups:
        row = [f'{x:{specifier}}' for x in grp]
        print(f"    {', '.join(row)},")
    print("};")


def print_array(name, dtype, vals, width=2, n=8):
    print_array_impl(
        name=name,
        dtype=dtype,
        vals=vals,
        specifier=f"{width}d",
        n=n,
        const_specifier='constexpr',
    )


tw_board = xlate_board(tw_board)
dw_board = xlate_board(dw_board)
tl_board = xlate_board(tl_board)
dl_board = xlate_board(dl_board)

print("#pragma once")
print("")
print("// Generated file!! Run ./gen_tables.py to re-generate")
print("")
print(f"constexpr int NumRows = {N_ROWS};")
print(f"constexpr int NumCols = {N_COLS};")
print(f"constexpr int NumSquares = {N_SQUARES};")
print("")
print_array(
    name='triple_word_squares',
    dtype='int',
    vals=tw_board,
    width=1,
    n=16,
)
print("")
print_array(
    name='double_word_squares',
    dtype='int',
    vals=dw_board,
    width=1,
    n=16,
)
print("")
print_array(
    name='triple_letter_squares',
    dtype='int',
    vals=tl_board,
    width=1,
    n=16,
)
print("")
print_array(
    name='double_letter_squares',
    dtype='int',
    vals=dl_board,
    width=1,
    n=16,
)

square_colors = []
for col in range(N_COLS):
    for row in range(N_ROWS):
        col_name = col
        row_name = row+1
        name = (col_name, row_name)
        if name in double_words:
            square_colors.append('DoubleWordSquareColor')
        elif name in triple_words:
            square_colors.append('TripleWordSquareColor')
        elif name in double_letters:
            square_colors.append('DoubleLetterSquareColor')
        elif name in triple_letters:
            square_colors.append('TripleLetterSquareColor')
        else:
            square_colors.append('EmptySquareColor')
print("")
print_array_impl(
    name='default_square_colors',
    dtype='ImVec4',
    vals=square_colors,
    specifier="23s",
    n=4,
    const_specifier='',
)
