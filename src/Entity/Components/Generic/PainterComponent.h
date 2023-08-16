#ifndef PAINTER_COMPONENT_H
#define PAINTER_COMPONENT_H

#include "Tile.h"
#include "vec2.h"

struct PainterComponent {
    bool canPaint = true;
    TileStatus paintColor = TileStatus::NOVAL;
    bool requestsPaint = false;
    strb::vec2 paintPos = {-1.f, -1.f};
};

#endif