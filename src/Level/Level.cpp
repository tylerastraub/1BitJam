#include "Level.h"
#include "EntityRegistry.h"
#include "TileSpritesheetHelper.h"
#include "TransformComponent.h"

void Level::allocateTilemap(int width, int height) {
    if(width < 1 || height < 1) return;
    _tilemapWidth = width;
    _tilemapHeight = height;
    std::vector<std::vector<Tile>> tilemap;
    for(int y = 0; y < _tilemapHeight; ++y) {
        std::vector<Tile> row;
        for(int x = 0; x < _tilemapWidth; ++x) {
            row.push_back(Tile{});
        }
        tilemap.push_back(row);
    }
    _tilemap = tilemap;
}


void Level::spawnPrefabs() {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _prefabs) {
        ecs->reregisterEntity(ent);
    }
}

void Level::render(int xOffset, int yOffset) {
    auto playerPos = EntityRegistry::getInstance()->getComponent<TransformComponent>(_playerId).position;
    if(_tileset == nullptr) return;
    for(int x = 0; x < _tilemapWidth; ++x) {
        for(int y = 0; y < _tilemapHeight; ++y) {
            if((x + 1) * _tileSize + xOffset < 0 || x * _tileSize + xOffset > _tilemapWidth * _tileSize ||
               (y + 1) * _tileSize + yOffset < 0 || y * _tileSize + yOffset > _tilemapHeight * _tileSize) {
                continue;
               }
            Tile t = getTileAt(x, y);
            if(t.status == TileStatus::LIGHT) TileSpritesheetHelper::updateSpritesheetRect(this, {(float) x, (float) y});
            if(t.spritesheetRect.w == 0 && t.spritesheetRect.h == 0) continue;

            _tileset->setTileWidth(t.spritesheetRect.w);
            _tileset->setTileHeight(t.spritesheetRect.h);
            _tileset->setTileIndex(t.spritesheetRect.x, t.spritesheetRect.y);
            // in future, these properties could be added to tile struct
            _tileset->setIsAnimated(false);

            _tileset->render(x * _tileSize + xOffset, y * _tileSize + yOffset, t.spritesheetRect.w, t.spritesheetRect.h);
        }
    }
}

void Level::updatePaintTiles() {
    _numOfPaintableTiles = 0;
    _numOfPaintedTiles = 0;
    _numOfPaintedBonusTiles = 0;
    _numOfPaintableBonusTiles = 0;
    for(size_t x = 0; x < _tilemapWidth; ++x) {
        for(size_t y = 0; y < _tilemapHeight; ++y) {
            if(getTileAt(x, y).type == TileType::GROUND) {
                ++_numOfPaintableTiles;
                if(getTileAt(x, y).status == TileStatus::LIGHT) {
                    ++_numOfPaintedTiles;
                }
            }
            else if(getTileAt(x, y).type == TileType::BONUS) {
                ++_numOfPaintableBonusTiles;
                if(getTileAt(x, y).status == TileStatus::LIGHT) {
                    ++_numOfPaintedBonusTiles;
                }
            }
        }
    }
}

void Level::setTilemap(std::vector<std::vector<Tile>> tilemap) {
    _tilemap = tilemap;
    _tilemapHeight = _tilemap.size();
    if(_tilemap.size() > 0) {
        _tilemapWidth = _tilemap[0].size();
    }
    updatePaintTiles();
}

void Level::setLightMap(std::shared_ptr<LightMap> lMap) {
    _lMap = lMap;
}

void Level::setTileSize(int tileSize) {
    _tileSize = tileSize;
}

void Level::setTileAt(int x, int y, Tile tile) {
    if(x >= 0 && x < _tilemapWidth && y >= 0 && y < _tilemapHeight) {
        // painted tiles update
        Tile oldTile = getTileAt(x, y);
        if(oldTile.type == TileType::GROUND &&
           oldTile.status == TileStatus::DARK &&
           tile.type == TileType::GROUND &&
           tile.status == TileStatus::LIGHT) {
            ++_numOfPaintedTiles;
        }
        else if(oldTile.type == TileType::GROUND &&
                oldTile.status == TileStatus::LIGHT &&
                tile.type == TileType::GROUND &&
                tile.status == TileStatus::DARK) {
            --_numOfPaintedTiles;
        }
        else if(oldTile.type == TileType::BONUS &&
           oldTile.status == TileStatus::DARK &&
           tile.type == TileType::BONUS &&
           tile.status == TileStatus::LIGHT) {
            ++_numOfPaintedBonusTiles;
        }
        else if(oldTile.type == TileType::BONUS &&
                oldTile.status == TileStatus::LIGHT &&
                tile.type == TileType::BONUS &&
                tile.status == TileStatus::DARK) {
            --_numOfPaintedBonusTiles;
        }
        // then actually set the tile
        _tilemap[y][x] = tile;
    }
}

void Level::setTileset(Spritesheet* tileset) {
    _tileset = tileset;
}

void Level::setPlayerId(Entity player) {
    _playerId = player;
}

void Level::addPrefab(Entity entity) {
    _prefabs.push_back(entity);
}

void Level::setPaintGoalPercent(float paintGoalPercent) {
    _paintGoalPercent = paintGoalPercent;
}

void Level::setNextLevel(std::string nextLevel) {
    _nextLevel = nextLevel;
}

void Level::setIsLevelComplete(bool isLevelComplete) {
    _isLevelComplete = isLevelComplete;
}

Tile Level::getTileAt(int x, int y) {
    if(x >= 0 && x < _tilemapWidth && y >= 0 && y < _tilemapHeight) {
        return _tilemap[y][x];
    }
    return Tile{TileType::NOVAL, TileStatus::NOVAL, {0, 0, 0, 0}};
}

int Level::getTileSize() {
    return _tileSize;
}

int Level::getTilemapWidth() {
    return _tilemapWidth;
}

int Level::getTilemapHeight() {
    return _tilemapHeight;
}

Entity Level::getPlayerId() {
    return _playerId;
}

LightMap* Level::getLightMap() {
    return _lMap.get();
}

std::pair<int, int> Level::getPaintedTileStatus() {
    return std::make_pair(_numOfPaintedTiles, _numOfPaintableTiles);
}

std::pair<int, int> Level::getBonusTileStatus() {
    return std::make_pair(_numOfPaintedBonusTiles, _numOfPaintableBonusTiles);
}

float Level::getPaintGoalPercent() {
    return _paintGoalPercent;
}

std::string Level::getNextLevel() {
    return _nextLevel;
}

std::string Level::getLevelResults(Timer timer, int enemiesDefeated, std::string actionInputString) {
    std::string tilesPainted = "Tiles: " + std::to_string(_numOfPaintedTiles) + "/" + std::to_string(_numOfPaintableTiles) + '\n';
    std::string bonusTiles = "Bonus: " + std::to_string(_numOfPaintedBonusTiles) + "/" + std::to_string(_numOfPaintableBonusTiles) + '\n';
    std::string enemies = "Enemies Painted: " + std::to_string(enemiesDefeated) + '\n';
    std::string timeCompleted = "Time: " + timer.getTimerAsString() + '\n';
    std::string next = "Press " + actionInputString + " to advance";
    
    return "== LEVEL COMPLETE ==\n" + tilesPainted + bonusTiles + enemies + timeCompleted + '\n' + next;
}

bool Level::isPaintGoalMet() {
    if(_numOfPaintedTiles >= _numOfPaintableTiles * _paintGoalPercent) {
        for(size_t x = 0; x < _tilemapWidth; ++x) {
            for(size_t y = 0; y < _tilemapHeight; ++y) {
                Tile stairs = getTileAt(x, y);
                if(stairs.type == TileType::STAIRS_DOWN && stairs.status == TileStatus::LOCKED) {
                    stairs.status = TileStatus::UNLOCKED;
                    stairs.spritesheetRect = {5, 4, 16, 16};
                    _tilemap[y][x] = stairs;
                }
            }
        }
        return true;
    }
    return false;
}

bool Level::isLevelComplete() {
    return _isLevelComplete;
}