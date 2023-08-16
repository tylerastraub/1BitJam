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
#include "CollisionComponent.h"
#include "TileFlipComponent.h"
#include "PainterComponent.h"
// Prefabs
#include "Player.h"

#include <chrono>

std::mt19937 RandomGen::randEng{(unsigned int) std::chrono::system_clock::now().time_since_epoch().count()};

/**
 * TODO:
 * - Determine final plan for painting mechanic
 * - Remove tile flip/revamp lock system
 * - Add basic enemies
 * - Add level generation (or create premade levels)
 * - Add items/level entities (torch?)
 * - Animate lock selection
*/

bool GameState::init() {
    auto ecs = EntityRegistry::getInstance();
    ecs->init();

    // Input
    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    // Level init
    Tile d = {TileType::GROUND, TileStatus::DARK, {0, 0, 16, 16}}; //dark tile
    Tile l = {TileType::GROUND, TileStatus::LIGHT, {1, 0, 16, 16}}; //light tile
    Tile e = {TileType::WALL, TileStatus::NOVAL, {2, 0, 16, 16}}; //empty tile
    /**
     * wall tiles. key is as follows:
     * 1 2 3
     * 4 5 6
     * 7 8 9
     * so w1 is the top left wall tile.
    */
    Tile w1 = {TileType::WALL, TileStatus::NOVAL, {0, 1, 16, 16}}; 
    Tile w2 = {TileType::WALL, TileStatus::NOVAL, {1, 1, 16, 16}}; 
    Tile w3 = {TileType::WALL, TileStatus::NOVAL, {2, 1, 16, 16}}; 
    Tile w4 = {TileType::WALL, TileStatus::NOVAL, {0, 2, 16, 16}}; 
    Tile w5 = {TileType::WALL, TileStatus::NOVAL, {1, 2, 16, 16}}; 
    Tile w6 = {TileType::WALL, TileStatus::NOVAL, {2, 2, 16, 16}}; 
    Tile w7 = {TileType::WALL, TileStatus::NOVAL, {0, 3, 16, 16}}; 
    Tile w8 = {TileType::WALL, TileStatus::NOVAL, {1, 3, 16, 16}}; 
    Tile w9 = {TileType::WALL, TileStatus::NOVAL, {2, 3, 16, 16}};
    _level.allocateTilemap(12, 12);
    _level.setTileset(SpritesheetRegistry::getSpritesheet(SpritesheetID::DEFAULT_TILESET));
    _level.setTileSize(16);
    _level.setPlayerId(_player);
    _level.setTilemap({
        {e, w8, w8, w8, w8, w8, w8, w8, w8, w8, w8, e},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, w1, w2, w3, d, w4},
        {w6, d, d, d, d, d, d, w4, e, w6, d, w4},
        {w6, d, d, d, d, d, d, w4, e, w6, d, w4},
        {w6, d, d, d, d, d, d, w7, w8, w9, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, w5, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {w6, d, d, d, d, d, d, d, d, d, d, w4},
        {e, w2, w2, w2, w2, w2, w2, w2, w2, w2, w2, e},
    });

    // Entity init
    initSystems();
    initPrefabs();

    // Camera
    auto& pTransform = ecs->getComponent<TransformComponent>(_player);
    auto& pRender = ecs->getComponent<RenderComponent>(_player);
    _cameraSystem->setCurrentCameraOffset(pTransform.position.x * _level.getTileSize() + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        pTransform.position.y * _level.getTileSize() + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->setGoalCameraOffset(pTransform.position.x * _level.getTileSize() + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        pTransform.position.y * _level.getTileSize() + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->update(1.f);
    _renderOffset = {_cameraSystem->getCurrentCameraOffset().x, _cameraSystem->getCurrentCameraOffset().x};

    return true;
}

void GameState::tick(float timescale) {
    auto ecs = EntityRegistry::getInstance();

    _scriptSystem->update(timescale);

    _inputSystem->update();

    _tileFlipSystem->update(&_level);

    _collisionSystem->update(timescale, &_level);

    _physicsSystem->update(timescale);

    _paintSystem->update(&_level);

    _renderSystem->update(timescale);

    // Camera
    auto& pTransform = ecs->getComponent<TransformComponent>(_player);
    auto& pRender = ecs->getComponent<RenderComponent>(_player);
    _cameraSystem->setGoalCameraOffset((int) (pTransform.position.x * _level.getTileSize()) + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        (int) (pTransform.position.y * _level.getTileSize()) + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->update(timescale);

    _renderOffset = _cameraSystem->getCurrentCameraOffset();
}

void GameState::render() {
    SDL_SetRenderDrawColor(getRenderer(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(getRenderer());

    _level.render((int) _renderOffset.x, (int) _renderOffset.y);

    _renderSystem->render(getRenderer(), (int) _renderOffset.x, (int) _renderOffset.y);

    _tileFlipSystem->render((int) _renderOffset.x, (int) _renderOffset.y, &_level);

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
    
    sig.reset();
    _collisionSystem = ecs->registerSystem<CollisionSystem>();
    sig.set(ecs->getComponentType<CollisionComponent>(), true);
    sig.set(ecs->getComponentType<PhysicsComponent>(), true);
    sig.set(ecs->getComponentType<TransformComponent>(), true);
    ecs->setSystemSignature<CollisionSystem>(sig);
    
    sig.reset();
    _tileFlipSystem = ecs->registerSystem<TileFlipSystem>();
    _tileFlipSystem->_audioPlayer = getAudioPlayer();
    sig.set(ecs->getComponentType<TileFlipComponent>(), true);
    ecs->setSystemSignature<TileFlipSystem>(sig);
    
    sig.reset();
    _paintSystem = ecs->registerSystem<PaintSystem>();
    _paintSystem->_audioPlayer = getAudioPlayer();
    sig.set(ecs->getComponentType<PainterComponent>(), true);
    ecs->setSystemSignature<PaintSystem>(sig);
}

void GameState::initPrefabs() {
    auto ecs = EntityRegistry::getInstance();

    _player = prefab::Player::create(3, 3);
}