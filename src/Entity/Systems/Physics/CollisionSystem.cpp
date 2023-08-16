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
        if(transform.goalPosition.x != transform.position.x) {
            Tile tile = level->getTileAt(transform.goalPosition.x, transform.goalPosition.y);
            if(tile.type == TileType::WALL) {
                if(transform.position.x > transform.goalPosition.x) {
                    collision.collidingLeft = true;
                    collision.collidingRight = false;
                }
                else if(transform.position.x < transform.goalPosition.x) {
                    collision.collidingRight = true;
                    collision.collidingLeft = false;
                }
                transform.goalPosition.x = transform.position.x;
            }
            else {
                collision.collidingLeft = false;
                collision.collidingRight = false;
            }
        }
        // Y collision check
        if(transform.goalPosition.y != transform.position.y) {
            Tile tile = level->getTileAt(transform.goalPosition.x, transform.goalPosition.y);
            if(tile.type == TileType::WALL) {
                if(transform.position.y > transform.goalPosition.y) {
                    collision.collidingUp = true;
                    collision.collidingDown = false;
                }
                else if(transform.position.y < transform.goalPosition.y) {
                    collision.collidingDown = true;
                    collision.collidingUp = false;
                }
                transform.goalPosition.y = transform.position.y;
            }
            else {
                collision.collidingUp = false;
                collision.collidingDown = false;
            }
        }
    }
}