#include "PhysicsSystem.h"
#include "EntityRegistry.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"

#include <iostream>
#include <algorithm>

void PhysicsSystem::update(float timescale) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& transform = ecs->getComponent<TransformComponent>(ent);

        transform.lastPosition = transform.position; // always update this since last position is based on tile position previous turn
        if(physics.velocity.x != 0.f || physics.velocity.y != 0.f) {
            transform.position += physics.velocity;
            physics.velocity = {0.f, 0.f};
        }
    }
}

void PhysicsSystem::moveToZero(float &value, float amount) {
    if(value != 0.f) {
        if(value > 0.f) {
            value = (value > amount) ? value - amount : 0.f;
        }
        else {
            value = (std::abs(value) > amount) ? value + amount : 0.f;
        }
    }
}