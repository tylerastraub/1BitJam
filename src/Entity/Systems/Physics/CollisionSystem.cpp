#include "CollisionSystem.h"
#include "EntityRegistry.h"
#include "CollisionComponent.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "PlayerComponent.h"

#include <iostream>
#include <cmath>

void CollisionSystem::update(float timescale, Level* level) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& collision = ecs->getComponent<CollisionComponent>(ent);
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& transform = ecs->getComponent<TransformComponent>(ent);
        Tile tile;

        // X collision check
        if(transform.goalPosition.x != transform.position.x) {
            tile = level->getTileAt(transform.goalPosition.x, transform.goalPosition.y);
            if(tile.type == TileType::WALL || (tile.entityOnTile != ent && tile.entityOnTile >= 0)) {
                if(transform.position.x > transform.goalPosition.x) {
                    collision.collidingLeft = true;
                    collision.collidingRight = false;
                }
                else if(transform.position.x < transform.goalPosition.x) {
                    collision.collidingRight = true;
                    collision.collidingLeft = false;
                }
                transform.goalPosition.x = transform.position.x;
                collision.lastCollision = tile;
            }
            else {
                collision.collidingLeft = false;
                collision.collidingRight = false;
            }
        }
        // Y collision check
        if(transform.goalPosition.y != transform.position.y) {
            tile = level->getTileAt(transform.goalPosition.x, transform.goalPosition.y);
            if(tile.type == TileType::WALL || (tile.entityOnTile != ent && tile.entityOnTile >= 0)) {
                if(transform.position.y > transform.goalPosition.y) {
                    collision.collidingUp = true;
                    collision.collidingDown = false;
                }
                else if(transform.position.y < transform.goalPosition.y) {
                    collision.collidingDown = true;
                    collision.collidingUp = false;
                }
                transform.goalPosition.y = transform.position.y;
                collision.lastCollision = tile;
            }
            else {
                collision.collidingUp = false;
                collision.collidingDown = false;
            }
        }

        // keep track of which tiles have entities on them
        tile = level->getTileAt(transform.lastPosition.x, transform.lastPosition.y);
        tile.entityOnTile = -1;
        level->setTileAt(transform.lastPosition.x, transform.lastPosition.y, tile);
        tile = level->getTileAt(transform.position.x, transform.position.y);
        tile.entityOnTile = ent;
        level->setTileAt(transform.position.x, transform.position.y, tile);
        if(transform.position != transform.goalPosition) {
            tile = level->getTileAt(transform.goalPosition.x, transform.goalPosition.y);
            tile.entityOnTile = ent;
            level->setTileAt(transform.goalPosition.x, transform.goalPosition.y, tile);
        }

        // Check what tile entity is stationary on and if we should act on that
        if(transform.position.x == std::floor(transform.position.x) &&
           transform.position.y == std::floor(transform.position.y)) {
            if(ecs->hasComponent<PlayerComponent>(ent)) {
                tile = level->getTileAt(transform.position.x, transform.position.y);
                if(tile.type == TileType::STAIRS_DOWN && tile.status == TileStatus::UNLOCKED) {
                    level->setIsLevelComplete(true);
                }
            }
        }
    }
}