#include <cicero/cicero.h>
#include "cicero_types.h"
#include <string.h>
#include <assert.h>


internal rack_tile char_to_rack_tile(char tile) {
    assert((CICERO_TILE_A <= tile && tile <= CICERO_TILE_Z) || tile == CICERO_TILE_BLANK);
    return tile == CICERO_TILE_BLANK ? 26 : tile - CICERO_TILE_A;
}

void cicero_make_rack(cicero_rack *rack, const char *const tiles)
{
    memset(rack, 0, sizeof(*rack));
    for (const char *tile = tiles; *tile != '\0'; ++tile) {
        assert((CICERO_TILE_A <= *tile && *tile <= CICERO_TILE_Z) || *tile == CICERO_TILE_BLANK ||
               *tile == CICERO_TILE_UNKNOWN);
        cicero_rack_add_tile(rack, *tile);
    }
}

void cicero_rack_add_tile(cicero_rack* rack, char tile)
{
    if (tile == CICERO_TILE_UNKNOWN) {
        return;
    }
    assert(('A' <= tile && tile <= 'Z') || tile == CICERO_TILE_BLANK);
    rack->tiles[char_to_rack_tile(tile)]++;
}
