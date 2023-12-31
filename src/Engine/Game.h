#ifndef GAME_H
#define GAME_H

#include "State.h"
#include "Audio.h"
#include "Settings.h"

#include <memory>

class Game {
public:
    Game(const char * windowTitle);
    ~Game();

    bool init();
    bool loadResources();
    void startGameLoop();
    void exit();

private:
    const char * _windowTitle;
    const char * _tinyTextFontPath = "res/font/04b03.ttf";
    const char * _smallTextFontPath = "res/font/edit-undo.brk.ttf";
    const char * _mediumTextFontPath = "res/font/MunroSmall.ttf";
    const char * _largeTextFontPath = "res/font/edit-undo.brk.ttf"; // temp

    const int GAME_WIDTH = 160;
    const int GAME_HEIGHT = 90;
    int _renderScale = 4;

    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
    SDL_GameController* _controller = nullptr;

    bool _exitFlag = false;

    State* _currentState = nullptr;
    State* _nextState = nullptr;

    // Resources
    std::unordered_map<TextSize, std::shared_ptr<Text>> _text;
    std::unique_ptr<Audio> _audioPlayer = nullptr;
    std::unique_ptr<Settings> _settings = nullptr;
};

#endif