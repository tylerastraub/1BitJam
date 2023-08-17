#include "DeathSystem.h"
#include "EntityRegistry.h"
#include "HealthComponent.h"
#include "StateComponent.h"
#include "DirectionComponent.h"
#include "RenderComponent.h"

void DeathSystem::update(float timescale) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& health = ecs->getComponent<HealthComponent>(ent);
        if(health.points < 1) {
            auto& state = ecs->getComponent<StateComponent>(ent);
            state.state = EntityState::DEAD;
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
                }
            }
        }
    }
}