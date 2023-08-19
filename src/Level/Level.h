#ifndef LEVEL_H
#define LEVEL_H

#include "Tile.h"
#include "Spritesheet.h"
#include "vec2.h"
#include "LightMap.h"
#include "Timer.h"
#include "Text.h"
#include "PowerupComponent.h"

#include <vector>
#include <cstdint>
#include <memory>

using Entity = std::uint16_t;

struct LevelText {
    strb::vec2 pos = {-1, -1};
    std::string text = "";
};

class Level {
public:
    Level() = default;
    ~Level() = default;

    void allocateTilemap(int width, int height);
    void spawnPrefabs();
    void render(int xOffset, int yOffset, Text* text);
    void updatePaintTiles();

    void setTilemap(std::vector<std::vector<Tile>> tilemap);
    void setLightMap(std::shared_ptr<LightMap> lightMap);
    void setTileSize(int tileSize);
    void setTileAt(int x, int y, Tile tile);
    void setTileset(Spritesheet* tileset);
    void setPlayerId(Entity player);
    void addPrefab(Entity entity);
    void addText(int x, int y, std::string text);
    void setPaintGoalPercent(float paintGoalPercent);
    void setNextLevel(std::string nextLevel);
    void setIsLevelComplete(bool isLevelComplete);
    void setBonusPowerup(PowerupComponent bonusPowerup);
    void setBonusMessage(std::string bonusMessage);

    Tile getTileAt(int x, int y);
    int getTileSize();
    int getTilemapWidth();
    int getTilemapHeight();
    Entity getPlayerId();
    LightMap* getLightMap();
    /**
     * Returns the number of painted tiles (pair.first) and how many tiles can be painted in total (pair.second).
    */
    std::pair<int, int> getPaintedTileStatus();
    /**
     * Returns the number of painted bonus tiles (pair.first) and how many bonus tiles can be painted in total (pair.second).
    */
    std::pair<int, int> getBonusTileStatus();
    float getPaintGoalPercent();
    std::string getNextLevel();
    std::string getLevelResults(Timer timer, int enemiesDefeated, std::string actionInputString);
    bool isPaintGoalMet();
    bool isLevelComplete();
    PowerupComponent getBonusPowerup();
    std::string getBonusMessage();

private:

    std::shared_ptr<LightMap> _lMap = nullptr;
    std::vector<std::vector<Tile>> _tilemap;
    int _tilemapWidth = 0;
    int _tilemapHeight = 0;
    int _tileSize = 16;
    Spritesheet* _tileset = nullptr;

    std::vector<Entity> _prefabs;
    Entity _playerId;
    std::vector<LevelText> _levelText = {};

    int _numOfPaintedTiles = 0;
    int _numOfPaintableTiles = 0;
    int _numOfPaintedBonusTiles = 0;
    int _numOfPaintableBonusTiles = 0;

    float _paintGoalPercent = 1.f; // percent of tiles needed to paint to advance to next level
    std::string _nextLevel = "";
    bool _isLevelComplete = false;
    PowerupComponent _bonusPowerup;
    std::string _bonusMessage = "";

};

#endif