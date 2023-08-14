#include "GameState.h"
#include "RandomGen.h"
#include "SpritesheetRegistry.h"
#include "EntityRegistry.h"
// Components
#include "InputComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "ScriptComponent.h"
#include "SpritesheetPropertiesComponent.h"
#include "DirectionComponent.h"
#include "PhysicsComponent.h"
// Prefabs
#include "Player.h"

#include <chrono>

std::mt19937 RandomGen::randEng{(unsigned int) std::chrono::system_clock::now().time_since_epoch().count()};

bool GameState::init() {
    EntityRegistry::getInstance()->init();

    // Input
    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    // Level init
    Tile d = {TileType::GROUND, {0, 0, 16, 16}}; //dark tile
    Tile l = {TileType::GROUND, {1, 0, 16, 16}}; //light tile
    Tile e = {TileType::WALL, {2, 0, 16, 16}}; //empty tile
    /**
     * wall tiles. key is as follows:
     * 1 2 3
     * 4 5 6
     * 7 8 9
     * so w1 is the top left wall tile.
    */
    Tile w1 = {TileType::WALL, {0, 1, 16, 16}}; 
    Tile w2 = {TileType::WALL, {1, 1, 16, 16}}; 
    Tile w3 = {TileType::WALL, {2, 1, 16, 16}}; 
    Tile w4 = {TileType::WALL, {0, 2, 16, 16}}; 
    Tile w5 = {TileType::WALL, {1, 2, 16, 16}}; 
    Tile w6 = {TileType::WALL, {2, 2, 16, 16}}; 
    Tile w7 = {TileType::WALL, {0, 3, 16, 16}}; 
    Tile w8 = {TileType::WALL, {1, 3, 16, 16}}; 
    Tile w9 = {TileType::WALL, {2, 3, 16, 16}};
    _level.allocateTilemap(12, 12);
    _level.setTileset(SpritesheetRegistry::getSpritesheet(SpritesheetID::DEFAULT_TILESET));
    _level.setTileSize(16);
    _level.setPlayerId(_player);
    _level.setTilemap({
        {e, w8, w8, w8, w8, w8, w8, w8, w8, w8, w8, e},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {e, w2, w2, w2, w2, w2, w2, w2, w2, w2, w2, e},
    });

    // Entity init
    initSystems();
    initPrefabs();

    return true;
}

void GameState::tick(float timescale) {
    _scriptSystem->update(timescale);

    _inputSystem->update();

    _physicsSystem->update(timescale);

    _renderSystem->update(timescale);
}

void GameState::render() {
    SDL_SetRenderDrawColor(getRenderer(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(getRenderer());

    _level.render((int) _renderOffset.x, (int) _renderOffset.y);

    _renderSystem->render(getRenderer(), (int) _renderOffset.x, (int) _renderOffset.y);

    SDL_RenderPresent(getRenderer());
}

void GameState::handleControllerButtonInput(SDL_Event e) {
    _controller->updateButtonInputs(e);
}

void GameState::handleControllerAxisInput(SDL_Event e) {
    _controller->updateAxisInputs(e);
}

void GameState::handleMouseInput(SDL_Event e) {
    _mouse->updateInput(e, _renderOffset.x, _renderOffset.y);
}

void GameState::initSystems() {
    auto ecs = EntityRegistry::getInstance();
    Signature sig;

    sig.reset();
    _inputSystem = ecs->registerSystem<InputSystem>();
    _inputSystem->init(_keyboard.get(), _controller.get(), getSettings());
    sig.set(ecs->getComponentType<InputComponent>(), true);
    ecs->setSystemSignature<InputSystem>(sig);

    sig.reset();
    _renderSystem = ecs->registerSystem<RenderSystem>();
    _renderSystem->setRenderBounds(getGameSize());
    sig.set(ecs->getComponentType<RenderComponent>(), true);
    ecs->setSystemSignature<RenderSystem>(sig);
    
    sig.reset();
    _scriptSystem = ecs->registerSystem<ScriptSystem>();
    sig.set(ecs->getComponentType<ScriptComponent>(), true);
    ecs->setSystemSignature<ScriptSystem>(sig);
    
    sig.reset();
    _cameraSystem = ecs->registerSystem<CameraSystem>();
    _cameraSystem->setGameSize(getGameSize().x, getGameSize().y);
    _cameraSystem->setLevelSize(_level.getTilemapWidth() * _level.getTileSize(),
        _level.getTilemapHeight() * _level.getTileSize());
    ecs->setSystemSignature<CameraSystem>(sig);
    
    sig.reset();
    _physicsSystem = ecs->registerSystem<PhysicsSystem>();
    sig.set(ecs->getComponentType<PhysicsComponent>(), true);
    sig.set(ecs->getComponentType<TransformComponent>(), true);
    ecs->setSystemSignature<PhysicsSystem>(sig);
}

void GameState::initPrefabs() {
    auto ecs = EntityRegistry::getInstance();

    _player = prefab::Player::create(3, 3);
}