#include "NavigationSystem.h"
#include "EntityRegistry.h"
#include "NavigationComponent.h"
#include "TransformComponent.h"

void NavigationSystem::setLevel(Level* level) {
    _level = level;
}

void NavigationSystem::onEntityCreated(Entity entity) {
    auto ecs = EntityRegistry::getInstance();
    auto& navigation = ecs->getComponent<NavigationComponent>(entity);
    navigation.level = *_level;
    ecs->addWatcher(this, entity);
}

void NavigationSystem::onEntityDelete(Entity entity)  {
    auto ecs = EntityRegistry::getInstance();
    auto& transform = ecs->getComponent<TransformComponent>(entity);
    Tile tile = _level->getTileAt(transform.position.x, transform.position.y);
    tile.entityOnTile = -1;
    _level->setTileAt(transform.position.x, transform.position.y, tile);
    if(transform.position != transform.goalPosition) {
        tile = _level->getTileAt(transform.goalPosition.x, transform.goalPosition.y);
        tile.entityOnTile = -1;
        _level->setTileAt(transform.goalPosition.x, transform.goalPosition.y, tile);
    }
}