#include "GameState.h"
#include "RandomGen.h"
#include "SpritesheetRegistry.h"
#include "EntityRegistry.h"
#include "LevelParser.h"
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
#include "Scrubber.h"

#include <chrono>

std::mt19937 RandomGen::randEng{(unsigned int) std::chrono::system_clock::now().time_since_epoch().count()};

/**
 * TODO:
 * - Figure out bonus tile purpose
 * - Add text for when goal is met, 100% of tiles painted, and bonus 100%
 * - Fix bug where inputs are carrying over between states (and inputPressed is getting read even though input remains down)
 * - Add more enemies/items
 *     - Teleporting enemy that shoots out black paint in AOE around it after teleport
 *         - Teleports on hit ?
 *     - Paint emitter that shoots out paint when you hit it
 *     - Smudge spawner
 * - Add sounds, MUSIC???
 * - Add menu
 * - Add tutorial level
*/

bool GameState::init() {
    // Input
    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    // ECS init
    auto ecs = EntityRegistry::getInstance();
    ecs->init();

    // Level/Systems init
    _level = LevelParser::parseLevelFromTmx(getLevelFilePath(), SpritesheetID::DEFAULT_TILESET);
    _level.updatePaintTiles();
    initSystems();
    _level.spawnPrefabs();

    // Camera
    auto& pTransform = ecs->getComponent<TransformComponent>(_player);
    auto& pRender = ecs->getComponent<RenderComponent>(_player);
    _cameraSystem->setCurrentCameraOffset(pTransform.position.x * _level.getTileSize() + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        pTransform.position.y * _level.getTileSize() + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->setGoalCameraOffset(pTransform.position.x * _level.getTileSize() + pRender.renderQuadOffset.x + pRender.renderQuad.w / 2 - getGameSize().x / 2,
        pTransform.position.y * _level.getTileSize() + pRender.renderQuadOffset.y + pRender.renderQuad.h / 2 - getGameSize().y / 2);
    _cameraSystem->update(1.f);
    _renderOffset = {_cameraSystem->getCurrentCameraOffset().x, _cameraSystem->getCurrentCameraOffset().x};

    // Dialogue box
    _dialogueBox.setAudio(getAudioPlayer());
    _dialogueBox.setIsEnabled(false);
    _dialogueBox.setReadSpeed(ReadSpeed::SLOW);
    _dialogueBox.setText(getText(TextSize::TINY));
    _dialogueBox.reset();

    // Timer
    _timer.changeToStopwatch();
    _timer.setTimer(0);
    _timer.reset();

    return true;
}

void GameState::tick(float timescale) {
    auto ecs = EntityRegistry::getInstance();

    if(_level.isLevelComplete()) {
        if(_dialogueBox.getTimeActive() == 0) {
            _dialogueBox.setString(_level.getLevelResults(_timer, _deathSystem->getNumOfDeaths(), getSettings()->getStringKeyboardControlForInputEvent(InputEvent::ACTION)));
            _dialogueBox.setIsEnabled(true);
        }
        if(_keyboard->isKeyPressed(getSettings()->getScancode(InputEvent::ACTION))) {
            if(_dialogueBox.isTextFullyDisplayed()) {
                _dialogueBox.advanceDialogue();
            }
            else {
                _dialogueBox.setTextFullyDisplayed(true);
            }
        }
        if(!_dialogueBox.isEnabled()) {
            GameState* nextState = new GameState();
            nextState->setLevelFilePath(_level.getNextLevel());
            setNextState(nextState);
        }
        _dialogueBox.tick(timescale);
        _keyboard->updateInputs();
        _controller->updateInputs();
        return;
    }

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
    std::pair<int, int> bonusStatus = _level.getBonusTileStatus();
    // std::cout << "Painted tiles: " << paintStatus.first << "/" << paintStatus.second << " - Bonus tiles: " << bonusStatus.first << "/" << bonusStatus.second << std::endl;

    // Timer
    _timer.update(timescale);
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
    // Paint goal render
    int paintGoalY = paintMeterY + 3 + PERCENT_PAINTED_MAX_HEIGHT * (1.f - _level.getPaintGoalPercent());
    SDL_Rect paintGoalLine = {2, paintGoalY, 4, 1};
    SDL_RenderFillRect(getRenderer(), &paintGoalLine);

    // Dialogue box
    if(_dialogueBox.isEnabled()) {
        _dialogueBox.render(getGameSize().x / 2 - 50, 5);
    }

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
    std::vector<strb::vec2> path = {
        {1, 1},
        {3, 1},
        {3, 2},
        {6, 2},
        {6, 1},
        {10, 1},
        {10, 10},
        {8, 10},
        {8, 9},
        {6, 9},
        {6, 10},
        {5, 10},
        {5, 9},
        {3, 9},
        {3, 10},
        {1, 10},
    };
    prefab::Scrubber::create(1, 1, path);
}