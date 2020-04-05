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

def xlate_basename(x):
    if x == '?':
        return 'Blank'
    elif x == ' ':
        return 'Empty'
    else:
        return x


def xlate_name(x):
    return f'Tile::{xlate_basename(x)}'


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


print("enum class SquareIndex : int {")
for col in range(15):
    for row in range(15):
        letter = chr(ord('A') + col)
        name = f'{letter}{row+1}'
        val = col*15 + row
        print(f"    {name:3s} = {val:3d},")
print("};")


# TODO: organize into rows of 15
print("const char* const SquareNames[225+1] = {")
for i in range(225):
    col = i % 15
    row = i // 15
    letter = chr(ord('A') + col)
    name = f'{letter}{row+1}'
    print(f'    "{name:>3s}",')
print(f'    "Invalid",')
print("};")
