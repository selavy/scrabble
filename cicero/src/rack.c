#include <cicero/cicero.h>
#include "cicero_types.h"
#include <string.h>
#include <assert.h>


internal rack_tile char_to_rack_tile(char tile) {
    assert(('A' <= tile && tile <= 'Z') || tile == CICERO_BLANK_TILE);
    return tile == CICERO_BLANK_TILE ? 26 : tile - 'A';
}

void cicero_make_rack(cicero_rack *rack, const char *const tiles)
{
    memset(rack, 0, sizeof(*rack));
    for (const char *tile = tiles; *tile != '\0'; ++tile) {
        assert(('A' <= *tile && *tile <= 'Z') || *tile == ' ' || *tile == '?');
        cicero_rack_add_tile(rack, *tile);
    }
}

void cicero_rack_add_tile(cicero_rack* rack, char tile)
{
    if (tile == CICERO_UNKNOWN_TILE) {
        return;
    }
    assert(('A' <= tile && tile <= 'Z') || tile == CICERO_BLANK_TILE);
    rack->tiles[char_to_rack_tile(tile)]++;
}
