#ifndef TILE_SPRITESHEET_HELPER_H
#define TILE_SPRITESHEET_HELPER_H

#include "Level.h"

#include <bitset>

struct TileSpritesheetHelper {
public:
    static void updateSpritesheetRect(Level* level, strb::vec2 tilePos) {
        Tile tile = level->getTileAt(tilePos.x, tilePos.y);
        switch(tile.type) {
            case TileType::GROUND: {
                if(tile.status == TileStatus::LIGHT) {
                    std::bitset<4> bitmask = getBitmaskValueForTile(level, tilePos);
                    tile.spritesheetRect = getLightGroundSpritesheetRect(bitmask);
                    level->setTileAt(tilePos.x, tilePos.y, tile);
                }
                break;
            }
            case TileType::WALL: {
                std::bitset<4> bitmask = getBitmaskValueForTile(level, tilePos);
                tile.spritesheetRect = getWallSpritesheetRect(bitmask);
                level->setTileAt(tilePos.x, tilePos.y, tile);
                break;
            }
            default:
                break;
        }
    }

private:
    static SDL_Rect getLightGroundSpritesheetRect(std::bitset<4> bitmask) {
        if(bitmask == 0b0000) {
            // no surrounding tiles
            return {1, 0, 16, 16};
        }
        else if(bitmask == 0b0001) {
            // tile to north
            return {4, 2, 16, 16};
        }
        else if(bitmask == 0b0010) {
            // tile to east
            return {4, 3, 16, 16};
        }
        else if(bitmask == 0b0011) {
            // tiles to north and east
            return {3, 1, 16, 16};
        }
        else if(bitmask == 0b0100) {
            // tile to south
            return {4, 1, 16, 16};
        }
        else if(bitmask == 0b0101) {
            // tiles to north and south
            return {4, 0, 16, 16};
        }
        else if(bitmask == 0b0110) {
            // tiles to south and east
            return {3, 0, 16, 16};
        }
        else if(bitmask == 0b0111) {
            // tiles to north, south, and east
            return {4, 0, 16, 16};
        }
        else if(bitmask == 0b1000) {
            // tile to west
            return {5, 3, 16, 16};
        }
        else if(bitmask == 0b1001) {
            // tiles to north and west
            return {5, 1, 16, 16};
        }
        else if(bitmask == 0b1010) {
            // tiles to east and west
            return {4, 0, 16, 16};
        }
        else if(bitmask == 0b1011) {
            // tiles to north, east, and west
            return {4, 0, 16, 16};
        }
        else if(bitmask == 0b1100) {
            // tiles to south and west
            return {5, 0, 16, 16};
        }
        else if(bitmask == 0b1101) {
            // tiles to north, south, and west
            return {4, 0, 16, 16};
        }
        else if(bitmask == 0b1110) {
            // tiles to south, east, and west
            return {4, 0, 16, 16};
        }
        else if(bitmask == 0b1111) {
            // tiles on all sides
            return {4, 0, 16, 16};
        }
        return {0, 0, 16, 16};
    }

    static SDL_Rect getWallSpritesheetRect(std::bitset<4> bitmask) {
        if(bitmask == 0b0000) {
            // no surrounding tiles
            return {1, 2, 16, 16};
        }
        else if(bitmask == 0b0001) {
            // tile to north
            return {3, 4, 16, 16};
        }
        else if(bitmask == 0b0010) {
            // tile to east
            return {2, 5, 16, 16};
        }
        else if(bitmask == 0b0011) {
            // tiles to north and east
            return {0, 3, 16, 16};
        }
        else if(bitmask == 0b0100) {
            // tile to south
            return {3, 2, 16, 16};
        }
        else if(bitmask == 0b0101) {
            // tiles to north and south
            return {3, 3, 16, 16};
        }
        else if(bitmask == 0b0110) {
            // tiles to south and east
            return {0, 1, 16, 16};
        }
        else if(bitmask == 0b0111) {
            // tiles to north, south, and east
            return {0, 2, 16, 16};
        }
        else if(bitmask == 0b1000) {
            // tile to west
            return {4, 5, 16, 16};
        }
        else if(bitmask == 0b1001) {
            // tiles to north and west
            return {2, 3, 16, 16};
        }
        else if(bitmask == 0b1010) {
            // tiles to east and west
            return {3, 5, 16, 16};
        }
        else if(bitmask == 0b1011) {
            // tiles to north, east, and west
            return {1, 3, 16, 16};
        }
        else if(bitmask == 0b1100) {
            // tiles to south and west
            return {2, 1, 16, 16};
        }
        else if(bitmask == 0b1101) {
            // tiles to north, south, and west
            return {2, 2, 16, 16};
        }
        else if(bitmask == 0b1110) {
            // tiles to south, east, and west
            return {1, 1, 16, 16};
        }
        else if(bitmask == 0b1111) {
            // tiles on all sides
            return {0, 0, 16, 16};
        }
        return {0, 0, 16, 16};
    }

    static std::bitset<4> getBitmaskValueForTile(Level* level, strb::vec2 tilePos) {
        strb::vec2 neighbors[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        std::bitset<4> bitmask;
        Tile mainTile = level->getTileAt(tilePos.x, tilePos.y);
        for(size_t i = 0; i < 4; ++i) {
            strb::vec2 neighbor = neighbors[i];
            Tile neighborTile = level->getTileAt(tilePos.x + neighbor.x, tilePos.y + neighbor.y);
            if(mainTile.type == neighborTile.type && mainTile.status == neighborTile.status) {
                bitmask.set(i, true);
            }
        }
        return bitmask;
    }
};

#endif