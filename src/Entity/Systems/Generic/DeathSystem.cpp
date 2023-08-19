#include "DeathSystem.h"
#include "EntityRegistry.h"
#include "HealthComponent.h"
#include "StateComponent.h"
#include "DirectionComponent.h"
#include "RenderComponent.h"

void DeathSystem::update(float timescale) {
    auto ecs = EntityRegistry::getInstance();

    auto entities = _entities;
    for(auto ent : entities) {
        auto& health = ecs->getComponent<HealthComponent>(ent);
        if(health.points < 1) {
            auto& state = ecs->getComponent<StateComponent>(ent);
            auto& dir = ecs->getComponent<DirectionComponent>(ent);
            state.state = EntityState::DEAD;
            dir.direction = Direction::SOUTH;
            if(!health.dead) {
                health.dead = true;
                health.points = 0;
                ecs->getComponent<RenderComponent>(ent).flicker = true;
            }
            if(_entityTimers.find(ent) == _entityTimers.end()) {
                _entityTimers[ent] = 0;
            }
            else {
                _entityTimers[ent] += timescale * 1000.f;
                if(_entityTimers[ent] > _despawnTime) {
                    ecs->destroyEntity(ent);
                    _entityTimers.erase(ent);
                    ++_deaths;
                    break; // TEMP SOLUTION!!! in reality need to keep better track of iterator and move it back one spot when entity is deleted
                }
            }
        }
    }
}

int DeathSystem::getNumOfDeaths() {
    return _deaths;
}