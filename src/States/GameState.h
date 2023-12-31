#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "State.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"
#include "Level.h"
#include "Timer.h"
#include "DialogueBox.h"
// Systems
#include "InputSystem.h"
#include "RenderSystem.h"
#include "ScriptSystem.h"
#include "CameraSystem.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"
#include "TileFlipSystem.h"
#include "PaintSystem.h"
#include "NavigationSystem.h"
#include "DeathSystem.h"

#include <memory>

class GameState: public State {
public:
    GameState() = default;
    ~GameState() = default;

    bool init() override;
    void tick(float timescale) override;
    void render() override;
    void handleKeyboardInput(SDL_Event e) override {};
    void handleControllerButtonInput(SDL_Event e) override;
    void handleControllerAxisInput(SDL_Event e) override;
    void handleMouseInput(SDL_Event e) override;

private:
    void initSystems();
    void initPrefabs();

    void addInfoString(std::string info);

    std::unique_ptr<Keyboard> _keyboard = nullptr;
    std::unique_ptr<Mouse> _mouse = nullptr;
    std::unique_ptr<Controller> _controller = nullptr;

    Level _level;

    DialogueBox _dialogueBox;

    Entity _player;

    std::shared_ptr<InputSystem> _inputSystem = nullptr;
    std::shared_ptr<RenderSystem> _renderSystem = nullptr;
    std::shared_ptr<ScriptSystem> _scriptSystem = nullptr;
    std::shared_ptr<CameraSystem> _cameraSystem = nullptr;
    std::shared_ptr<PhysicsSystem> _physicsSystem = nullptr;
    std::shared_ptr<CollisionSystem> _collisionSystem = nullptr;
    std::shared_ptr<TileFlipSystem> _tileFlipSystem = nullptr;
    std::shared_ptr<PaintSystem> _paintSystem = nullptr;
    std::shared_ptr<NavigationSystem> _navigationSystem = nullptr;
    std::shared_ptr<DeathSystem> _deathSystem = nullptr;

    strb::vec2 _renderOffset = {0.f, 0.f};

    Timer _timer;
    float _paintPercent = 0.f; // percent from 0-1

    int _infoDisplayTime = 3000;
    int _currentInfoDisplayTimer = 0;
    std::vector<std::string> _infoStrings = {};
    bool _tileGoalMet = false;
    bool _bonusGoalMet = false;
    bool _allTilesGoalMet = false;
};

#endif