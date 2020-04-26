#!/usr/bin/env python

FREQS = (
    ('A',  9),
    ('B',  2),
    ('C',  2),
    ('D',  4),
    ('E', 12),
    ('F',  2),
    ('G',  3),
    ('H',  2),
    ('I',  9),
    ('J',  1),
    ('K',  1),
    ('L',  4),
    ('M',  2),
    ('N',  6),
    ('O',  8),
    ('P',  2),
    ('Q',  1),
    ('R',  6),
    ('S',  4),
    ('T',  6),
    ('U',  4),
    ('V',  2),
    ('W',  2),
    ('X',  1),
    ('Y',  2),
    ('Z',  1),
    ('?',  2),
    (' ',  0),
)

VALUES = {
    '?': 0,
    'E': 1,
    'A': 1,
    'I': 1,
    'O': 1,
    'N': 1,
    'R': 1,
    'T': 1,
    'L': 1,
    'S': 1,
    'U': 1,
    'D': 2,
    'G': 2,
    'B': 3,
    'C': 3,
    'M': 3,
    'P': 3,
    'F': 4,
    'H': 4,
    'V': 4,
    'W': 4,
    'Y': 4,
    'K': 5,
    'J': 8,
    'X': 8,
    'Q': 10,
    'Z': 10,
}

def xlate_basename(x):
    if x == '?':
        return 'Blank'
    elif x == ' ':
        return 'Empty'
    else:
        return x


def xlate_name(x):
    return f'Tile::{xlate_basename(x)}'


def print_values(vals, specifier, n):
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


def print_array_impl(name, dtype, vals, specifier, n, const_specifier=''):
    const_specifier = const_specifier.strip()
    if const_specifier:
        const_specifier += ' '
    print(f"{const_specifier}std::array<{dtype}, {len(vals)}> {name} = {{")
    print_values(vals=vals, specifier=specifier, n=n)
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

NAME = { x: xlate_name(x) for x, _ in FREQS }

total_tiles = sum([x[1] for x in FREQS])
print(f"constexpr int NumTotalTiles = {total_tiles};")

print("enum class Tile {")
for i, (letter, _) in enumerate(FREQS):
    if letter == '?':
        letter = 'BLANK'
    print(f"    {letter} = {i},")
print("};")

for letter, freq in FREQS:
    name = NAME[letter]
    print(f"bag[static_cast<std::size_t>({name})] = {freq};")


print(f"const char* const TileNames[{len(FREQS)}] = {{")
for tile, _ in FREQS:
    print(f'    "{tile}",')
print("};")


square_indices = []
for col in range(15):
    for row in range(15):
        letter = chr(ord('A') + col)
        name = f'{letter}{row+1}'
        val = col*15 + row
        square_indices.append(f"{name:>3s}")
print("enum class Sq : int {")
print_values(
    vals=square_indices,
    specifier="s",
    n=15
)
print("};")


# TODO: organize into rows of 15
square_names = []
for i in range(225):
    col = i % 15
    row = i // 15
    letter = chr(ord('A') + row)
    name = f'{letter}{col+1}'
    final = f'"{name:>3s}"'
    square_names.append(final)
square_names.append('"INVALID"')

print_array_impl(
    name='SquareNames',
    dtype='const char* const',
    vals=square_names,
    specifier="s",
    n=15,
    const_specifier='constexpr',
)

# print("constexpr std::array<const char* const, 225+1> SquareNames = {")
# for i in range(225):
#     col = i % 15
#     row = i // 15
#     letter = chr(ord('A') + row)
#     name = f'{letter}{col+1}'
#     print(f'    "{name:>3s}",')
# print(f'    "Invalid",')
# print("};")

letter_values = []
for i in range(26):
    c = chr(ord('A') + i)
    letter_values.append(VALUES[c])
for i in range(26):
    # c = chr(ord('A') + i)
    letter_values.append(VALUES['?'])
letter_values.append(VALUES['?'])

# letter_values = []
# for i in range(128):
#     c = chr(i)
#     if 'a' <= c <= 'z':
#         c = '?'
#     letter_values.append(VALUES.get(c, 0))

print_array(
    name='letter_values',
    dtype='int',
    vals=letter_values,
    width=2,
    n=16,
)

