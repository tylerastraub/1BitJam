#include "MainMenuState.h"
#include "GameState.h"
#include "FileIO.h"

#include <algorithm>

bool MainMenuState::init() {
    _keyboard = std::make_unique<Keyboard>();
    _mouse = std::make_unique<Mouse>(getRenderScale(), getRenderScale());
    _controller = std::make_unique<Controller>();

    _gui = std::make_unique<GUI>(1, 6);
    _gui->setWrapX(false);
    _gui->setWrapY(true);

    std::vector<std::string> completedLevels = FileIO::readFile("res/completedlevels.txt");
    
    std::shared_ptr<Button> levelZeroButton = std::make_shared<Button>(getText(TextSize::TINY));
    levelZeroButton->setId("levelZeroButton");
    levelZeroButton->setValue("0");
    levelZeroButton->setPos(0, 0);
    levelZeroButton->setCenterAligned(true);
    _gui->addGUIElement(levelZeroButton);
    
    std::shared_ptr<Button> levelOneButton = std::make_shared<Button>(getText(TextSize::TINY));
    levelOneButton->setId("levelOneButton");
    levelOneButton->setValue("1");
    levelOneButton->setPos(0, 1);
    levelOneButton->setCenterAligned(true);
    if(std::find(completedLevels.begin(), completedLevels.end(), "res/tiled/level0.tmx") != completedLevels.end()) {
        _gui->addGUIElement(levelOneButton);
    }
    
    std::shared_ptr<Button> levelTwoButton = std::make_shared<Button>(getText(TextSize::TINY));
    levelTwoButton->setId("levelTwoButton");
    levelTwoButton->setValue("2");
    levelTwoButton->setPos(0, 2);
    levelTwoButton->setCenterAligned(true);
    if(std::find(completedLevels.begin(), completedLevels.end(), "res/tiled/level1.tmx") != completedLevels.end()) {
        _gui->addGUIElement(levelTwoButton);
    }
    
    std::shared_ptr<Button> levelThreeButton = std::make_shared<Button>(getText(TextSize::TINY));
    levelThreeButton->setId("levelThreeButton");
    levelThreeButton->setValue("3");
    levelThreeButton->setPos(0, 3);
    levelThreeButton->setCenterAligned(true);
    if(std::find(completedLevels.begin(), completedLevels.end(), "res/tiled/level2.tmx") != completedLevels.end()) {
        _gui->addGUIElement(levelThreeButton);
    }
    
    std::shared_ptr<Button> levelFourButton = std::make_shared<Button>(getText(TextSize::TINY));
    levelFourButton->setId("levelFourButton");
    levelFourButton->setValue("4");
    levelFourButton->setPos(0, 4);
    levelFourButton->setCenterAligned(true);
    if(std::find(completedLevels.begin(), completedLevels.end(), "res/tiled/level3.tmx") != completedLevels.end()) {
        _gui->addGUIElement(levelFourButton);
    }
    
    std::shared_ptr<Button> levelFiveButton = std::make_shared<Button>(getText(TextSize::TINY));
    levelFiveButton->setId("levelFiveButton");
    levelFiveButton->setValue("5");
    levelFiveButton->setPos(0, 5);
    levelFiveButton->setCenterAligned(true);
    if(std::find(completedLevels.begin(), completedLevels.end(), "res/tiled/level4.tmx") != completedLevels.end()) {
        _gui->addGUIElement(levelFiveButton);
    }

    _gui->setColumnRenderWidth(0, getGameSize().x);
    _gui->setAllRowsRenderHeight(10);
    _gui->setCurrentSelection(0, 0);

    return true;
}

void MainMenuState::tick(float timescale) {
    if(_menuTimer < 100) {
        _menuTimer += timescale * 1000.f;
        return;
    }
    if(_keyboard->isKeyPressed(SDL_SCANCODE_SPACE) ||
       _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_A)) {
        std::string s = _gui->getCurrentSelection()->getId();
        if(s == "levelZeroButton") {
            GameState* game = new GameState();
            game->setLevelFilePath("res/tiled/level0.tmx");
            setNextState(game);
        }
        else if(s == "levelOneButton") {
            GameState* game = new GameState();
            game->setLevelFilePath("res/tiled/level1.tmx");
            setNextState(game);
        }
        else if(s == "levelTwoButton") {
            GameState* game = new GameState();
            game->setLevelFilePath("res/tiled/level2.tmx");
            setNextState(game);
        }
        else if(s == "levelThreeButton") {
            GameState* game = new GameState();
            game->setLevelFilePath("res/tiled/level3.tmx");
            setNextState(game);
        }
        else if(s == "levelFourButton") {
            GameState* game = new GameState();
            game->setLevelFilePath("res/tiled/level4.tmx");
            setNextState(game);
        }
        else if(s == "levelFiveButton") {
            GameState* game = new GameState();
            game->setLevelFilePath("res/tiled/level5.tmx");
            setNextState(game);
        }
    }

    if(_keyboard->isKeyPressed(SDL_SCANCODE_UP)) {
        _gui->changeSelection(0, -1);
    }
    if(_keyboard->isKeyPressed(SDL_SCANCODE_DOWN)) {
        _gui->changeSelection(0, 1);
    }

    _keyboard->updateInputs();
    _controller->updateInputs();
}

void MainMenuState::render() {
    SDL_SetRenderDrawColor(getRenderer(), 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(getRenderer());

    _gui->render(0, 16);

    Text* tinyText = getText(TextSize::TINY);
    tinyText->setString("Use arrow keys/SPACE to select");
    tinyText->render(getGameSize().x / 2 - tinyText->getWidth() / 2, getGameSize().y - tinyText->getHeight() - 1);

    Text* smallText = getText(TextSize::SMALL);
    smallText->setString("PAINTMAN");
    smallText->render(getGameSize().x / 2 - smallText->getWidth() / 4, 1);

    SDL_RenderPresent(getRenderer());
}

void MainMenuState::handleControllerButtonInput(SDL_Event e) {
    _controller->updateButtonInputs(e);
}

void MainMenuState::handleControllerAxisInput(SDL_Event e) {
    _controller->updateAxisInputs(e);
}

void MainMenuState::handleMouseInput(SDL_Event e) {
    _mouse->updateInput(e, _renderOffset.x, _renderOffset.y);
}