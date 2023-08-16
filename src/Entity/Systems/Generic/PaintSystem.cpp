#include "PaintSystem.h"
#include "EntityRegistry.h"
#include "TransformComponent.h"
#include "PainterComponent.h"

#include <cmath>

void PaintSystem::update(Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto transform = ecs->getComponent<TransformComponent>(ent);
        auto painter = ecs->getComponent<PainterComponent>(ent);
        Tile tile = level->getTileAt(std::roundf(transform.position.x), std::roundf(transform.position.y));
        if(tile.status != painter.paintColor) {
            tile.status = painter.paintColor;
            level->setTileAt(std::roundf(transform.position.x), std::roundf(transform.position.y), tile);
        }
    }
}