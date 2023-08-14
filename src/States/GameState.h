#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "State.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"
#include "Level.h"
// Systems
#include "InputSystem.h"
#include "RenderSystem.h"
#include "ScriptSystem.h"
#include "CameraSystem.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"

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

    std::unique_ptr<Keyboard> _keyboard = nullptr;
    std::unique_ptr<Mouse> _mouse = nullptr;
    std::unique_ptr<Controller> _controller = nullptr;

    Level _level;

    Entity _player;

    std::shared_ptr<InputSystem> _inputSystem = nullptr;
    std::shared_ptr<RenderSystem> _renderSystem = nullptr;
    std::shared_ptr<ScriptSystem> _scriptSystem = nullptr;
    std::shared_ptr<CameraSystem> _cameraSystem = nullptr;
    std::shared_ptr<PhysicsSystem> _physicsSystem = nullptr;
    std::shared_ptr<CollisionSystem> _collisionSystem = nullptr;

    SDL_FPoint _renderOffset = {0.f, 0.f};
};

#endif