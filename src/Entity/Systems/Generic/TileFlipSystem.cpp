#include "TileFlipSystem.h"
#include "EntityRegistry.h"
#include "TileFlipComponent.h"
#include "DirectionComponent.h"
#include "SpritesheetRegistry.h"
#include "TransformComponent.h"

void TileFlipSystem::update(float timescale, Level* level) {
    _animationTimer += timescale * 1000;
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& tileFlip = ecs->getComponent<TileFlipComponent>(ent);
        if(tileFlip.isFlippingTile) {
            tileFlip.isFlippingTile = false;
            Tile tile = level->getTileAt(tileFlip.tileFlipPos.x, tileFlip.tileFlipPos.y);
            // If tile status is NOVAL, then we don't do anything as it is not a flippable tile
            if(tile.status == TileStatus::DARK) tile.status = TileStatus::LIGHT;
            else if(tile.status == TileStatus::LIGHT) tile.status = TileStatus::DARK;
            tile.spritesheetRect = updateSpritesheetRectForTile(tile);
            level->setTileAt(tileFlip.tileFlipPos.x, tileFlip.tileFlipPos.y, tile);
            tileFlip.tileFlipPos = {-1.f, -1.f};
        }
    }
}

void TileFlipSystem::render(int xOffset, int yOffset, Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& tileFlip = ecs->getComponent<TileFlipComponent>(ent);
        if(tileFlip.isLocked) {
            if(!_wasLockedLastTick) _animationTimer = 0;
            auto transform = ecs->getComponent<TransformComponent>(ent);
            auto dir = ecs->getComponent<DirectionComponent>(ent);
            strb::vec2 offset = {0.f, 0.f};
            switch(dir.direction) {
                case Direction::NORTH:
                    offset.y -= 1;
                    break;
                case Direction::SOUTH:
                    offset.y += 1;
                    break;
                case Direction::WEST:
                    offset.x -= 1; 
                    break;
                case Direction::EAST:
                    offset.x += 1;
                    break;
            }
            Tile tile = level->getTileAt(transform.position.x + offset.x, transform.position.y + offset.y);
            if(tile.status == TileStatus::NOVAL) continue;
            int xIndex = (tile.status == TileStatus::DARK) ? 0 : 1;
            int yIndex = _animationTimer % 1000 / 500; // animation updates every 500ms
            Spritesheet* hud = SpritesheetRegistry::getSpritesheet(SpritesheetID::HUD);
            hud->setTileIndex(xIndex, yIndex);
            hud->setTileWidth(16);
            hud->setTileHeight(16);
            hud->render((transform.position.x + offset.x) * 16 + xOffset,
                (transform.position.y + offset.y) * 16 + yOffset, 16, 16);
        }
        _wasLockedLastTick = tileFlip.isLocked;
    }
}

SDL_Rect TileFlipSystem::updateSpritesheetRectForTile(Tile tile) {
    switch(tile.status) {
        case TileStatus::DARK:
            return {0, 0, 16, 16};
        case TileStatus::LIGHT:
            return {1, 0, 16, 16};
        default:
            return tile.spritesheetRect;
    }
}