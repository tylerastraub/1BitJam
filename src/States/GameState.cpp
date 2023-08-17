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
#include "NavigationComponent.h"
#include "HealthComponent.h"
// Prefabs
#include "Player.h"
#include "Smudge.h"

#include <chrono>

std::mt19937 RandomGen::randEng{(unsigned int) std::chrono::system_clock::now().time_since_epoch().count()};

/**
 * TODO:
 * - Make paint mechanic FUN!! Explode enemies to paint. Get powerups to paint more efficiently. Definitely don't require 100% paint coverage
 *     - Can move on to next floor only after painting certain amount
 * - Remove tile flip/revamp lock system
 * - Add basic enemies
 * - Add level generation (or create premade levels)
*/

bool GameState::init() {

    // Input
    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    // Level init
    Tile d = {TileType::GROUND, TileStatus::DARK, {0, 0, 16, 16}}; //dark tile
    Tile l = {TileType::GROUND, TileStatus::LIGHT, {1, 0, 16, 16}}; //light tile
    Tile e = {TileType::WALL, TileStatus::NOVAL, {2, 0, 16, 16}}; //empty tile
    Tile w = {TileType::WALL, TileStatus::NOVAL, {0, 0, 16, 16}}; // wall tile
    _level.allocateTilemap(12, 12);
    _level.setTileset(SpritesheetRegistry::getSpritesheet(SpritesheetID::DEFAULT_TILESET));
    _level.setTileSize(16);
    _level.setPlayerId(_player);
    _level.setTilemap({
        {e, w, w, w, w, w, w, w, w, w, w, e},
        {w, d, d, d, d, d, d, d, d, d, d, w},
        {w, d, d, d, d, d, d, w, w, w, d, w},
        {w, d, d, d, d, d, d, w, e, w, d, w},
        {w, d, d, d, d, d, d, w, e, w, d, w},
        {w, d, d, d, d, d, d, w, w, w, d, w},
        {w, d, d, d, d, d, d, d, d, d, d, w},
        {w, d, d, w, d, d, d, d, d, d, d, w},
        {w, d, d, d, d, d, d, d, d, d, d, w},
        {w, d, d, d, d, d, d, d, d, d, d, w},
        {w, d, d, d, d, d, d, d, d, d, d, w},
        {e, w, w, w, w, w, w, w, w, w, w, e},
    });

    // Entity init
    auto ecs = EntityRegistry::getInstance();
    ecs->init();
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

    _deathSystem->update(timescale);

    _inputSystem->update();

    _tileFlipSystem->update(timescale, &_level);

    _collisionSystem->update(timescale, &_level);

    _physicsSystem->update(timescale);

    _paintSystem->update(timescale, &_level);

    _renderSystem->update(timescale);

    // Camera
    auto& pTransform = ecs->getComponent<TransformComponent>(_player);
    auto& pRender = ecs->getComponent<RenderComponent>(_player);
    _cameraSystem->setGoalCameraOffset((int) (pTransform.position.x * _level.getTileSize()) + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        (int) (pTransform.position.y * _level.getTileSize()) + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->update(timescale);

    _renderOffset = _cameraSystem->getCurrentCameraOffset();

    // Paint
    std::pair<int, int> paintStatus = _level.getPaintedTileStatus();
    if(paintStatus.second != 0) _paintPercent = static_cast<float>(paintStatus.first) / static_cast<float>(paintStatus.second);
}

void GameState::render() {
    SDL_SetRenderDrawColor(getRenderer(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(getRenderer());

    _level.render((int) _renderOffset.x, (int) _renderOffset.y);

    _renderSystem->render(getRenderer(), (int) _renderOffset.x, (int) _renderOffset.y);

    _tileFlipSystem->render((int) _renderOffset.x, (int) _renderOffset.y, &_level);

    // Paint percent render
    Spritesheet* paintMeter = SpritesheetRegistry::getSpritesheet(SpritesheetID::PAINT_METER);
    int paintMeterY = getGameSize().y / 2.f - paintMeter->getHeight() / 2.f;
    paintMeter->render(0, paintMeterY, paintMeter->getWidth(), paintMeter->getHeight());
    SDL_SetRenderDrawColor(getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
    const int PERCENT_PAINTED_MAX_HEIGHT = 50;
    int percentPaintedHeight = PERCENT_PAINTED_MAX_HEIGHT * _paintPercent;
    if(percentPaintedHeight < 1) percentPaintedHeight = 1;
    SDL_Rect percentPaintedProgress = {3, paintMeterY + 3 + PERCENT_PAINTED_MAX_HEIGHT - percentPaintedHeight, 2, percentPaintedHeight};
    SDL_RenderFillRect(getRenderer(), &percentPaintedProgress);

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
    _scriptSystem->_audioPlayer = getAudioPlayer();
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
    
    sig.reset();
    _navigationSystem = ecs->registerSystem<NavigationSystem>();
    _navigationSystem->setLevel(&_level);
    sig.set(ecs->getComponentType<NavigationComponent>(), true);
    ecs->setSystemSignature<NavigationSystem>(sig);
    
    sig.reset();
    _deathSystem = ecs->registerSystem<DeathSystem>();
    sig.set(ecs->getComponentType<HealthComponent>(), true);
    ecs->setSystemSignature<DeathSystem>(sig);
}

void GameState::initPrefabs() {
    auto ecs = EntityRegistry::getInstance();

    _player = prefab::Player::create(3, 3);
    prefab::Smudge::create(6, 6);
}