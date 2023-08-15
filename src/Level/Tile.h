#ifndef TILE_H
#define TILE_H

#include <SDL.h>

enum class TileType {
    NOVAL = -1,
    GROUND,
    WALL,
    HAZARD,
};

enum class TileStatus {
    NOVAL = -1,
    DARK,
    LIGHT,
};

struct Tile {
    TileType type = TileType::NOVAL;
    TileStatus status = TileStatus::NOVAL;
    SDL_Rect spritesheetRect = {0, 0, 0, 0};
};

#endif