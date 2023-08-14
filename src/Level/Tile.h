#ifndef TILE_H
#define TILE_H

#include <SDL.h>

enum class TileType {
    NOVAL = -1,
    GROUND,
    WALL,
    HAZARD,
};

struct Tile {
    TileType type = TileType::NOVAL;
    SDL_Rect spritesheetRect = {0, 0, 0, 0};
};

#endif