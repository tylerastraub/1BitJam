#ifndef PAINTER_COMPONENT_H
#define PAINTER_COMPONENT_H

#include "Tile.h"

struct PainterComponent {
    bool canPaint = true;
    TileStatus paintColor = TileStatus::NOVAL;
};

#endif