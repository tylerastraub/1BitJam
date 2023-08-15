#ifndef TILE_FLIP_COMPONENT_H
#define TILE_FLIP_COMPONENT_H

#include "vec2.h"

struct TileFlipComponent {
    bool canFlipTiles = true;
    bool isFlippingTile = false;
    strb::vec2 tileFlipPos = {-1.f, -1.f};
    bool isLocked = false;
};

#endif