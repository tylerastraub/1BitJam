#include "NavigationSystem.h"
#include "EntityRegistry.h"
#include "NavigationComponent.h"

void NavigationSystem::setLevel(Level level) {
    _level = level;
}

void NavigationSystem::onEntityCreated(Entity entity) {
    auto ecs = EntityRegistry::getInstance();
    auto& navigation = ecs->getComponent<NavigationComponent>(entity);
    navigation.level = _level;
}