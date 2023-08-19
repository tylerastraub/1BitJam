#ifndef PAINTER_COMPONENT_H
#define PAINTER_COMPONENT_H

#include "Tile.h"
#include "vec2.h"

#include <vector>

struct PainterComponent {
    bool canPaint = true;
    TileStatus paintColor = TileStatus::NOVAL;
    bool requestsPaint = false;
    std::vector<strb::vec2> paintPos = {};
};

#endif