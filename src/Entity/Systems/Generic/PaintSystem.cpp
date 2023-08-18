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
        auto painter = ecs->getComponent<PainterComponent>(ent);
        Tile tile = level->getTileAt(painter.paintPos.x, painter.paintPos.y);
        if((tile.type == TileType::GROUND || tile.type == TileType::BONUS) &&
           tile.status != painter.paintColor &&
           painter.requestsPaint) {
            painter.requestsPaint = false;
            if(tile.entityOnTile < 0 || tile.entityOnTile == ent) {
                tile.status = painter.paintColor;
                level->setTileAt(painter.paintPos.x, painter.paintPos.y, tile);
                // if(tile.type == TileType::BONUS) _audioPlayer->playAudio(ent, AudioSound::PAINT_BONUS, 0.5f);
            }
            if(ecs->hasComponent<PaintAttackComponent>(ent)) {
                auto& paintAttack = ecs->getComponent<PaintAttackComponent>(ent);
                if(paintAttack.attacking) {
                    paintAttack.attacking = false;
                    // _audioPlayer->playAudio(ent, AudioSound::PAINT, 0.5f);
                    if(tile.entityOnTile >= 0) {
                        auto& health = ecs->getComponent<HealthComponent>(tile.entityOnTile);
                        --health.points;
                    }
                }
            }
        }
        // timer
        if(ecs->hasComponent<PaintAttackComponent>(ent)) {
            auto& paintAttack = ecs->getComponent<PaintAttackComponent>(ent);
            paintAttack.msSinceLastPaintAttack += timescale * 1000.f;
        }
    }
}