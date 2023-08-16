#ifndef TILE_FLIP_SYSTEM_H
#define TILE_FLIP_SYSTEM_H

#include "System.h"
#include "Level.h"

class TileFlipSystem : public System {
public:
    TileFlipSystem() = default;
    ~TileFlipSystem() = default;

    void update(float timescale, Level* level);
    void render(int xOffset, int yOffset, Level* level);

private:
    SDL_Rect updateSpritesheetRectForTile(Tile tile);

    unsigned int _animationTimer = 0;
    bool _wasLockedLastTick = false;

};

#endif