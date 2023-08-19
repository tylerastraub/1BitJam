#include "PaintSystem.h"
#include "EntityRegistry.h"
#include "TransformComponent.h"
#include "PainterComponent.h"
#include "PaintAttackComponent.h"
#include "HealthComponent.h"

#include <cmath>

void PaintSystem::update(float timescale, Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto transform = ecs->getComponent<TransformComponent>(ent);
        auto& painter = ecs->getComponent<PainterComponent>(ent);
        for(auto paintPos : painter.paintPos) {
            Tile tile = level->getTileAt(paintPos.x, paintPos.y);
            if((tile.type == TileType::GROUND || tile.type == TileType::BONUS) &&
            tile.status != painter.paintColor &&
            painter.requestsPaint) {
                if(tile.entityOnTile < 0 || tile.entityOnTile == ent) {
                    tile.status = painter.paintColor;
                    if(painter.paintColor == TileStatus::DARK) tile.spritesheetRect = {0, 0, 16, 16};
                    level->setTileAt(paintPos.x, paintPos.y, tile);
                    level->isPaintGoalMet();
                    // if(tile.type == TileType::BONUS) _audioPlayer->playAudio(ent, AudioSound::PAINT_BONUS, 0.5f);
                }
            }
            if(ecs->hasComponent<PaintAttackComponent>(ent)) {
                auto& paintAttack = ecs->getComponent<PaintAttackComponent>(ent);
                if(paintAttack.attacking) {
                    if(tile.entityOnTile >= 0) {
                        if(ecs->hasComponent<HealthComponent>(tile.entityOnTile)) {
                            auto& health = ecs->getComponent<HealthComponent>(tile.entityOnTile);
                            --health.points;
                            paintAttack.attacking = false;
                        }
                    }
                }
            }
        }
        painter.requestsPaint = false;
        if(ecs->hasComponent<PaintAttackComponent>(ent)) ecs->getComponent<PaintAttackComponent>(ent).attacking = false;
        painter.paintPos.clear();
        // timer
        if(ecs->hasComponent<PaintAttackComponent>(ent)) {
            auto& paintAttack = ecs->getComponent<PaintAttackComponent>(ent);
            paintAttack.msSinceLastPaintAttack += timescale * 1000.f;
        }
    }
}