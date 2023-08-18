#include "PhysicsSystem.h"
#include "EntityRegistry.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "HealthComponent.h"

#include <iostream>
#include <algorithm>

void PhysicsSystem::update(float timescale) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto oldVelocity = physics.velocity;
        
        if(ecs->hasComponent<HealthComponent>(ent)) {
            auto& health = ecs->getComponent<HealthComponent>(ent);
            if(health.points < 1) {
                physics.velocity = {0.f, 0.f};
                continue;
            }
        }

        auto& transform = ecs->getComponent<TransformComponent>(ent);

        transform.lastPosition = transform.position; // always update this since last position is based on tile position previous turn

        // checks for if entity is moving
        if(transform.position.x < transform.goalPosition.x) {
            // physics.velocity.x = ((1 - physics.moveSpeed.x) * transform.position.x) + (physics.moveSpeed.x * transform.goalPosition.x) - transform.position.x;
            physics.velocity.x = physics.moveSpeed.x;
        }
        else if(transform.position.x > transform.goalPosition.x) {
            // physics.velocity.x = ((1 - physics.moveSpeed.x) * transform.position.x) + (physics.moveSpeed.x * transform.goalPosition.x) - transform.position.x;
            physics.velocity.x = physics.moveSpeed.x * -1;
        }
        else {
            physics.velocity.x = 0.f;
        }
        if(transform.position.y < transform.goalPosition.y) {
            // physics.velocity.y = ((1 - physics.moveSpeed.y) * transform.position.y) + (physics.moveSpeed.y * transform.goalPosition.y) - transform.position.y;
            physics.velocity.y = physics.moveSpeed.y;
        }
        else if(transform.position.y > transform.goalPosition.y) {
            // physics.velocity.y = ((1 - physics.moveSpeed.y) * transform.position.y) + (physics.moveSpeed.y * transform.goalPosition.y) - transform.position.y;
            physics.velocity.y = physics.moveSpeed.y * -1;
        }
        else {
            physics.velocity.y = 0.f;
        }

        // checks for if entity is about to reach destination. if so, place entity right on goal position
        if(physics.velocity.x < 0) {
            if(transform.position.x - physics.moveSpeed.x < transform.goalPosition.x) {
                physics.velocity.x = 0;
                transform.position.x = transform.goalPosition.x;
            }
        }
        else if(physics.velocity.x > 0) {
            if(transform.position.x + physics.moveSpeed.x > transform.goalPosition.x) {
                physics.velocity.x = 0;
                transform.position.x = transform.goalPosition.x;
            }
        }
        if(physics.velocity.y < 0) {
            if(transform.position.y - physics.moveSpeed.y < transform.goalPosition.y) {
                physics.velocity.y = 0;
                transform.position.y = transform.goalPosition.y;
            }
        }
        else if(physics.velocity.y > 0) {
            if(transform.position.y + physics.moveSpeed.y > transform.goalPosition.y) {
                physics.velocity.y = 0;
                transform.position.y = transform.goalPosition.y;
            }
        }
        // finally, just move normally
        transform.position += physics.velocity;
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