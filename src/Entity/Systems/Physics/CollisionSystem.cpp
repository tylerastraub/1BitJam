#include "CollisionSystem.h"
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "EntityRegistry.h"

#include <iostream>

void CollisionSystem::update(float timescale, Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& collision = ecs->getComponent<CollisionComponent>(ent);
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& transform = ecs->getComponent<TransformComponent>(ent);

        // X collision check
        if(physics.velocity.x != 0.f) {
            Tile tile = level->getTileAt(transform.position.x + physics.velocity.x, transform.position.y);
            if(tile.type == TileType::WALL) {
                if(physics.velocity.x < 0.f) {
                    collision.collidingLeft = true;
                    collision.collidingRight = false;
                }
                else if(physics.velocity.x > 0.f) {
                    collision.collidingRight = true;
                    collision.collidingLeft = false;
                }
                physics.velocity.x = 0.f;
            }
            else {
                collision.collidingLeft = false;
                collision.collidingRight = false;
            }
        }
        // Y collision check
        if(physics.velocity.y != 0.f) {
            Tile tile = level->getTileAt(transform.position.x, transform.position.y + physics.velocity.y);
            if(tile.type == TileType::WALL) {
                if(physics.velocity.y < 0.f) {
                    collision.collidingUp = true;
                    collision.collidingDown = false;
                }
                else if(physics.velocity.y > 0.f) {
                    collision.collidingDown = true;
                    collision.collidingUp = false;
                }
                physics.velocity.y = 0.f;
            }
            else {
                collision.collidingUp = false;
                collision.collidingDown = false;
            }
        }
    }
}