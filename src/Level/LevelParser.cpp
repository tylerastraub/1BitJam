#include "LevelParser.h"
#include "FileIO.h"
#include "SpritesheetRegistry.h"
#include "EntityRegistry.h"
// Prefabs
#include "DialogueTrigger.h"
#include "PrefabSpawnTrigger.h"
#include "Player.h"
#include "Smudge.h"
#include "Scrubber.h"

#include <algorithm>
#include <iostream>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>

Level LevelParser::parseLevelFromTmx(std::string filePath, SpritesheetID spritesheetId) {
    Level level;
    level.setTileset(SpritesheetRegistry::getSpritesheet(spritesheetId));

    tmx::Map map;
    if(map.load(SDL_GetBasePath() + filePath)) {
        int tileSize = map.getTileSize().x;
        level.setTileSize(tileSize);
        level.allocateTilemap(map.getTileCount().x, map.getTileCount().y);
        const auto& layers = map.getLayers();

        // Tilesets
        strb::vec2 tilesetSize;
        std::vector<tmx::Property> tileProperties;
        const auto& tilesets = map.getTilesets();
        for(const auto& tileset : tilesets) {
            //read out tile set properties, load textures etc...
            tilesetSize.x = tileset.getImageSize().x;
            tilesetSize.y = tileset.getImageSize().y;
        }

        // Map layers
        for(const auto& layer : layers) {
            // Object layer
            if(layer->getType() == tmx::Layer::Type::Object) {
                const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
                const auto& objects = objectLayer.getObjects();
                if(layer->getName() == "objects") {
                    for(const auto& object : objects) {
                        // ============================== PREFABS ==============================
                        if(object.getName() == "player") {
                            Entity player = prefab::Player::create(object.getPosition().x / tileSize, object.getPosition().y / tileSize);
                            level.setPlayerId(player);
                            level.addPrefab(player);
                        }
                        else if(object.getName() == "smudge") {
                            Entity smudge = prefab::Smudge::create(object.getPosition().x / tileSize, object.getPosition().y / tileSize);
                            level.addPrefab(smudge);
                        }
                        else if(object.getName() == "scrubber") {
                            std::vector<strb::vec2> path = {};
                            for(auto prop : object.getProperties()) {
                                if(prop.getName() == "path" && prop.getType() == tmx::Property::Type::String) {
                                    path = parsePathFromString(prop.getStringValue());
                                }
                            }
                            Entity scrubber = prefab::Scrubber::create(object.getPosition().x / tileSize, object.getPosition().y / tileSize, path);
                            level.addPrefab(scrubber);
                        }
                        // ============================== TRIGGERS ==============================
                        else if(object.getName() == "trigger") {
                            if(object.getClass() == "dialogue") {
                                auto aabb = object.getAABB();
                                int conversationId = -1;
                                bool triggerOnce = true;
                                bool entityMustBeGrounded = false;
                                for(auto prop : object.getProperties()) {
                                    if(prop.getName() == "conversationId" && prop.getType() == tmx::Property::Type::Int) {
                                        conversationId = prop.getIntValue();
                                    }
                                    else if(prop.getName() == "triggerOnce" && prop.getType() == tmx::Property::Type::Boolean) {
                                        triggerOnce = prop.getBoolValue();
                                    }
                                    else if(prop.getName() == "entityMustBeGrounded" && prop.getType() == tmx::Property::Type::Boolean) {
                                        entityMustBeGrounded = prop.getBoolValue();
                                    }
                                }
                                Entity trigger = prefab::DialogueTrigger::create(
                                    {(int) aabb.left, (int) aabb.top, (int) aabb.width, (int) aabb.height},
                                    triggerOnce,
                                    entityMustBeGrounded,
                                    conversationId
                                );
                                level.addPrefab(trigger);
                            }
                            // ============================== PREFAB SPAWNS ==============================
                            else if(object.getClass() == "prefabSpawn") {
                                auto aabb = object.getAABB();
                                PrefabType prefabType = PrefabType::NOVAL;
                                strb::vec2 prefabSpawnPos = {0.f, 0.f};
                                std::string prefabValue = "";
                                bool triggerOnce = true;
                                bool entityMustBeGrounded = false;
                                for(auto prop : object.getProperties()) {
                                    if(prop.getName() == "prefabType" && prop.getType() == tmx::Property::Type::String) {
                                        prefabType = convertStringToPrefabType(prop.getStringValue());
                                    }
                                    else if(prop.getName() == "triggerOnce" && prop.getType() == tmx::Property::Type::Boolean) {
                                        triggerOnce = prop.getBoolValue();
                                    }
                                    else if(prop.getName() == "entityMustBeGrounded" && prop.getType() == tmx::Property::Type::Boolean) {
                                        entityMustBeGrounded = prop.getBoolValue();
                                    }
                                    else if(prop.getName() == "prefabSpawnXPos" && prop.getType() == tmx::Property::Type::Float) {
                                        prefabSpawnPos.x = prop.getFloatValue();
                                    }
                                    else if(prop.getName() == "prefabSpawnYPos" && prop.getType() == tmx::Property::Type::Float) {
                                        prefabSpawnPos.y = prop.getFloatValue();
                                    }
                                    else if(prop.getName() == "prefabValue" && prop.getType() == tmx::Property::Type::String) {
                                        prefabValue = prop.getStringValue();
                                    }
                                }
                                Entity trigger = prefab::PrefabSpawnTrigger::create(
                                    {(int) aabb.left, (int) aabb.top, (int) aabb.width, (int) aabb.height},
                                    triggerOnce,
                                    entityMustBeGrounded,
                                    prefabType,
                                    prefabSpawnPos,
                                    prefabValue
                                );
                                level.addPrefab(trigger);
                            }
                        }
                    }
                }
                else if(layer->getName() == "collisionmap") {
                    for(const auto& object : objects) {
                        int xPos = object.getPosition().x / level.getTileSize();
                        int yPos = object.getPosition().y / level.getTileSize();
                        Tile tile = level.getTileAt(xPos, yPos);
                        if(object.getClass() == "ground") {
                            tile.type = TileType::GROUND;
                            tile.status = TileStatus::DARK;
                        }
                        else if(object.getClass() == "wall") {
                            tile.type = TileType::WALL;
                        }
                        else if(object.getClass() == "hazard") {
                            tile.type = TileType::HAZARD;
                        }
                        else if(object.getClass() == "stairs_up") {
                            tile.type = TileType::STAIRS_UP;
                        }
                        else if(object.getClass() == "stairs_down") {
                            tile.type = TileType::STAIRS_DOWN;
                        }
                        else if(object.getClass() == "bonus") {
                            tile.type = TileType::BONUS;
                            tile.status = TileStatus::DARK;
                        }
                        level.setTileAt(xPos, yPos, tile);
                    }
                }
            }
            // Tile layers
            else if(layer->getType() == tmx::Layer::Type::Tile) {
                const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
                auto& tiles = tileLayer.getTiles();
                for(size_t y = 0; y < map.getTileCount().y; ++y) {
                    for(size_t x = 0; x < map.getTileCount().x; ++x) {
                        int tileIndex = x + y * map.getTileCount().x;
                        auto id = tiles[tileIndex].ID;
                        // a tile ID of 0 = empty tile. as a result, all tile ID's are incremented by 1
                        // so we need to account for that. but first, check for empty tile:
                        if(id == 0) continue;
                        --id;
                        Tile tile = level.getTileAt(x, y);
                        auto xIndex = id % (int) (tilesetSize.x / level.getTileSize());
                        auto yIndex = id / (int) (tilesetSize.x / level.getTileSize());
                        tile.spritesheetRect = {
                            (int) xIndex,
                            (int) yIndex,
                            level.getTileSize(),
                            level.getTileSize()
                        };
                        level.setTileAt(x, y, tile);
                    }
                }
            }
        }
    }
    else {
        std::cout << "Error: failed to load level '" << filePath << "'!" << std::endl;
    }

    return level;
}

PrefabType LevelParser::convertStringToPrefabType(std::string prefabTypeString) {
    PrefabType result = PrefabType::NOVAL;
    if(prefabTypeString == "ITEM") {
        result = PrefabType::PICKUP;
    }
    else if(prefabTypeString == "PROJECTILE") {
        result = PrefabType::PROJECTILE;
    }

    return result;
}

std::vector<strb::vec2> LevelParser::parsePathFromString(std::string pathString) {
    std::vector<strb::vec2> result = {};
    // remove spaces
    std::remove_if(pathString.begin(), pathString.end(), [](char c) {return c == ' ';});
    // then extra coords
    while(pathString.size() > 0) {
        strb::vec2 coord = {-1, -1};
        size_t coordsDelimeter = pathString.find('\n');
        if(coordsDelimeter == std::string::npos) coordsDelimeter = pathString.size();
        size_t xyDelimeter = pathString.find(',');
        coord.x = std::stoi(pathString.substr(0, xyDelimeter));
        coord.y = std::stoi(pathString.substr(xyDelimeter + 1, coordsDelimeter - xyDelimeter - 1));
        pathString.erase(0, coordsDelimeter + 1);
        result.push_back(coord);
    }

    return result;
}