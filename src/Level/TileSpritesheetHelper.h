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
                if(tile.status != TileStatus::LIGHT) {
                    tile.spritesheetRect = {0, 0, 16, 16};
                    level->setTileAt(tilePos.x, tilePos.y, tile);
                    return;
                }
                // light tile - lets find the right spritesheet rect
                std::bitset<4> bitmask = getBitmaskValueForTile(level, tilePos);
                if(bitmask == 0b0000) {
                    // no surrounding tiles
                    tile.spritesheetRect = {1, 0, 16, 16};
                }
                else if(bitmask == 0b0001) {
                    // tile to north
                    tile.spritesheetRect = {4, 2, 16, 16};
                }
                else if(bitmask == 0b0010) {
                    // tile to east
                    tile.spritesheetRect = {4, 3, 16, 16};
                }
                else if(bitmask == 0b0011) {
                    // tiles to north and east
                    tile.spritesheetRect = {3, 1, 16, 16};
                }
                else if(bitmask == 0b0100) {
                    // tile to south
                    tile.spritesheetRect = {4, 1, 16, 16};
                }
                else if(bitmask == 0b0101) {
                    // tiles to north and south
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                else if(bitmask == 0b0110) {
                    // tiles to south and east
                    tile.spritesheetRect = {3, 0, 16, 16};
                }
                else if(bitmask == 0b0111) {
                    // tiles to north, south, and east
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                else if(bitmask == 0b1000) {
                    // tile to west
                    tile.spritesheetRect = {5, 3, 16, 16};
                }
                else if(bitmask == 0b1001) {
                    // tiles to north and west
                    tile.spritesheetRect = {5, 1, 16, 16};
                }
                else if(bitmask == 0b1010) {
                    // tiles to east and west
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                else if(bitmask == 0b1011) {
                    // tiles to north, east, and west
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                else if(bitmask == 0b1100) {
                    // tiles to south and west
                    tile.spritesheetRect = {5, 0, 16, 16};
                }
                else if(bitmask == 0b1101) {
                    // tiles to north, south, and west
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                else if(bitmask == 0b1110) {
                    // tiles to south, east, and west
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                else if(bitmask == 0b1111) {
                    // tiles on all sides
                    tile.spritesheetRect = {4, 0, 16, 16};
                }
                level->setTileAt(tilePos.x, tilePos.y, tile);
                break;
            }
            case TileType::WALL:
                // TODO: we'll deal with this later if we want to
                break;
            default:
                break;
        }
    }

private:
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