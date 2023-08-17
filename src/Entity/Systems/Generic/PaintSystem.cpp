#include "PaintSystem.h"
#include "EntityRegistry.h"
#include "TransformComponent.h"
#include "PainterComponent.h"
#include "PaintAttackComponent.h"
#include "HealthComponent.h"

#include <cmath>

// TODO: put logic here to check if painting on tile is allowed (namely if there is an enemy there)
// also move all painting logic from setTileAt to in here
void PaintSystem::update(float timescale, Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto transform = ecs->getComponent<TransformComponent>(ent);
        auto painter = ecs->getComponent<PainterComponent>(ent);
        Tile tile = level->getTileAt(painter.paintPos.x, painter.paintPos.y);
        if(tile.type == TileType::GROUND &&
           tile.status != painter.paintColor &&
           painter.requestsPaint) {
            painter.requestsPaint = false;
            if(tile.entityOnTile < 0 || tile.entityOnTile == ent) {
                tile.status = painter.paintColor;
                level->setTileAt(painter.paintPos.x, painter.paintPos.y, tile);
                // _audioPlayer->playAudio(ent, AudioSound::PAINT, 0.5f);
            }
            if(ecs->hasComponent<PaintAttackComponent>(ent)) {
                auto& paintAttack = ecs->getComponent<PaintAttackComponent>(ent);
                if(paintAttack.attacking) {
                    paintAttack.attacking = false;
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