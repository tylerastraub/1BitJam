#include "Game.h"
#include "SpritesheetRegistry.h"
#include "GameState.h"
#include "MainMenuState.h"

#include <chrono>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cmath>

Game::Game(const char * windowTitle) : _windowTitle(windowTitle) {}

Game::~Game() {
    if(_currentState != nullptr) delete _currentState;
    if(_nextState != nullptr) delete _nextState;
}

bool Game::init() {
    bool windowCreatedSuccessfully = false;

    // SDL initialization
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL failed to initialize. SDL_Error: " << SDL_GetError() << std::endl;
    }
    else {
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
            std::cout << "Warning: Nearest pixel sampling not enabled!" << std::endl;
        }
        if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")) {
            std::cout << "Warning: Vsync not enabled!" << std::endl;
        }

        // Window and renderer initialization
        _settings = std::make_unique<Settings>();
        _settings->loadSettings("settings.cfg");
        _window = SDL_CreateWindow(_windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _settings->getVideoWidth(), _settings->getVideoHeight(), _settings->getVideoMode() | SDL_WINDOW_RESIZABLE);
        if(_window == nullptr)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else {
            _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
            if(_renderer == nullptr) {
                std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
            }
            else {
                SDL_RenderSetLogicalSize(_renderer, GAME_WIDTH, GAME_HEIGHT);
                if(SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND) == -1) {
                    std::cout << "Error: failed to set render draw blend mode to SDL_BLENDMODE_BLEND. SDL_Error: " << SDL_GetError() << std::endl;
                }
                // SDL_Image initialization
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init( imgFlags ) & imgFlags)) {
                    std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
                }
                if(TTF_Init() == -1) {
                    std::cout << "SDL_ttf could not be initialized! SDL_ttf Error: " << TTF_GetError() << std::endl;
                }
                else {
                    // Gamepad initialization
                    SDL_JoystickEventState(SDL_ENABLE);
                    if(SDL_IsGameController(0)) {
                        _controller = SDL_GameControllerOpen(0);
                        if(_controller == NULL) {
                            std::cout << "Error: Unable to open controller!" << std::endl;
                        }
                        else {
                            std::cout << "Controller connected: " << SDL_GameControllerName(_controller) << std::endl;
                            if(SDL_GameControllerAddMappingsFromFile("res/controllermappings.txt") == -1) {
                                std::cout << "Error loading controller mappings! SDL_Error: " << SDL_GetError() << std::endl;
                            }
                        }
                    }
                    else {
                        std::cout << "No controllers connected." << std::endl;
                    }
                    // Resource loading
                    if(!loadResources()) {
                        std::cout << "Could not load resources!" << std::endl;
                    }
                    else {
                        // State initialization
                        // _currentState = new GameState();
                        _currentState = new MainMenuState();
                        _currentState->setGameSize(GAME_WIDTH, GAME_HEIGHT);
                        _currentState->setRenderer(_renderer);
                        for(auto it : _text) {
                            _currentState->addText(it.first, it.second.get());
                        }
                        _currentState->setAudioPlayer(_audioPlayer.get());
                        _currentState->setSettings(_settings.get());
                        // _currentState->setLevelFilePath("res/tiled/level1.tmx");
                        _currentState->init();
                        SDL_ShowCursor(SDL_DISABLE);
                        windowCreatedSuccessfully = true;
                    }
                }
            }
        }
    }
    
    return windowCreatedSuccessfully;
}

bool Game::loadResources() {
    // Text
    std::shared_ptr<Text> tinyText = std::make_shared<Text>(_renderer);
    if(!tinyText->load(_tinyTextFontPath, 8)) {
        std::cout << "Error: Failed to load font '" << _tinyTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::TINY] = tinyText;
    
    std::shared_ptr<Text> smallText = std::make_shared<Text>(_renderer);
    if(!smallText->load(_smallTextFontPath, 14)) {
        std::cout << "Error: Failed to load font '" << _smallTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::SMALL] = smallText;

    std::shared_ptr<Text> mediumText = std::make_shared<Text>(_renderer);
    if(!mediumText->load(_mediumTextFontPath, 20)) {
        std::cout << "Error: Failed to load font '" << _mediumTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::MEDIUM] = mediumText;
    
    std::shared_ptr<Text> largeText = std::make_shared<Text>(_renderer);
    if(!largeText->load(_largeTextFontPath, 24)) {
        std::cout << "Error: Failed to load font '" << _largeTextFontPath << "'!" << std::endl;
        return false;
    }
    _text[TextSize::LARGE] = largeText;

    // Spritesheets
    std::shared_ptr<Spritesheet> defaultTileset = std::make_shared<Spritesheet>();
    if(!defaultTileset->load(_renderer, "res/spritesheet/default_tileset.png")) return false;
    defaultTileset->setTileWidth(16);
    defaultTileset->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::DEFAULT_TILESET, defaultTileset);

    std::shared_ptr<Spritesheet> dialogueSpritesheet = std::make_shared<Spritesheet>();
    if(!dialogueSpritesheet->load(_renderer, "res/spritesheet/dialogue_box.png")) return false;
    dialogueSpritesheet->setTileWidth(100);
    dialogueSpritesheet->setTileHeight(80);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::DIALOGUE_BOX, dialogueSpritesheet);
    
    std::shared_ptr<Spritesheet> playerSpritesheet = std::make_shared<Spritesheet>();
    if(!playerSpritesheet->load(_renderer, "res/spritesheet/player_spritesheet.png")) return false;
    playerSpritesheet->setTileWidth(16);
    playerSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::PLAYER, playerSpritesheet);
    
    std::shared_ptr<Spritesheet> hudSpritesheet = std::make_shared<Spritesheet>();
    if(!hudSpritesheet->load(_renderer, "res/spritesheet/hud_spritesheet.png")) return false;
    hudSpritesheet->setTileWidth(16);
    hudSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::HUD, hudSpritesheet);
    
    std::shared_ptr<Spritesheet> paintMeter = std::make_shared<Spritesheet>();
    if(!paintMeter->load(_renderer, "res/spritesheet/paint_meter.png")) return false;
    paintMeter->setTileWidth(8);
    paintMeter->setTileHeight(60);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::PAINT_METER, paintMeter);
    
    std::shared_ptr<Spritesheet> paintSplashSpritesheet = std::make_shared<Spritesheet>();
    if(!paintSplashSpritesheet->load(_renderer, "res/spritesheet/paint_splash_spritesheet.png")) return false;
    paintSplashSpritesheet->setTileWidth(16);
    paintSplashSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::PAINT_SPALSH, paintSplashSpritesheet);
    
    std::shared_ptr<Spritesheet> smudgeSpritesheet = std::make_shared<Spritesheet>();
    if(!smudgeSpritesheet->load(_renderer, "res/spritesheet/smudge_spritesheet.png")) return false;
    smudgeSpritesheet->setTileWidth(16);
    smudgeSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::SMUDGE, smudgeSpritesheet);
    
    std::shared_ptr<Spritesheet> scrubberSpritesheet = std::make_shared<Spritesheet>();
    if(!scrubberSpritesheet->load(_renderer, "res/spritesheet/scrubber_spritesheet.png")) return false;
    scrubberSpritesheet->setTileWidth(16);
    scrubberSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::SCRUBBER, scrubberSpritesheet);
    
    std::shared_ptr<Spritesheet> spreaderSpritesheet = std::make_shared<Spritesheet>();
    if(!spreaderSpritesheet->load(_renderer, "res/spritesheet/spreader_spritesheet.png")) return false;
    spreaderSpritesheet->setTileWidth(16);
    spreaderSpritesheet->setTileHeight(16);
    SpritesheetRegistry::addSpritesheet(SpritesheetID::SPREADER, spreaderSpritesheet);

    // Audio
    _audioPlayer = std::make_unique<Audio>();
    if(!_audioPlayer->addAudio(AudioSound::CHARACTER_BLIP, "res/audio/character_blip.wav")) return false;
    if(!_audioPlayer->addAudio(AudioSound::PAINT_ATTACK, "res/audio/paint1.wav")) return false;

    return true;
}

void Game::startGameLoop() {
    SDL_Event e;
    auto startTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds dTime = std::chrono::milliseconds(0); // deltaTime
    Uint32 millisecondCount = 0;
    Uint32 frames = 0;
    float frameWait = 1.f / 60.f;
    float frameRemainder = 0.f;
    while(_exitFlag == false) {
        // Event Handling
        while(SDL_PollEvent(&e) != 0) {
            switch(e.type) {
                case SDL_QUIT:
                    _exitFlag = true;
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_ESCAPE) {
                    _exitFlag = true;
                    }
                    else {
                        _currentState->handleKeyboardInput(e);
                    }
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                case SDL_CONTROLLERBUTTONUP:
                    _currentState->handleControllerButtonInput(e);
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    _currentState->handleControllerAxisInput(e);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    _currentState->handleMouseInput(e);
                    break;
                default:
                    break;
            }
        }

        if(_currentState->getNextState() != nullptr) {
            State* tempState = _currentState->getNextState();
            delete _currentState;
            _currentState = tempState;
            _currentState->setGameSize(GAME_WIDTH, GAME_HEIGHT);
            _currentState->setRenderer(_renderer);
            for(auto it : _text) {
                _currentState->addText(it.first, it.second.get());
            }
            _currentState->setAudioPlayer(_audioPlayer.get());
            _currentState->setSettings(_settings.get());
            _currentState->init();
        }

        // Settings changed
        if(_currentState->settingsChanged()) {
            _currentState->getSettings()->saveSettings();
            _settings->loadSettings("settings.cfg"); // this is a dumb hack since i don't wanna make a copy constructor
            SDL_SetWindowSize(_window, _settings->getVideoWidth(), _settings->getVideoHeight());
            if(_settings->getVideoMode() == SDL_WINDOW_FULLSCREEN) {
                SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
            }
            else {
                SDL_bool windowed = SDL_FALSE;
                if(_settings->getVideoMode() == SDL_WINDOW_SHOWN) windowed = SDL_TRUE;
                SDL_SetWindowBordered(_window, windowed);
                SDL_SetWindowResizable(_window, windowed);
            }
            SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            _currentState->completeSettingsChange();
        }

        dTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
        if(dTime.count() >= (frameWait * 1000.f - frameRemainder)) {
            startTime = std::chrono::high_resolution_clock::now();
            millisecondCount += dTime.count();
            frameRemainder += std::abs(frameWait * 1000.f - std::ceil(frameWait * 1000.f));
            if(frameRemainder > 1.f) frameRemainder = 0.f;
            _currentState->tick(frameWait);
            _currentState->render();
            frames++;
        }

        if(millisecondCount >= 1000) {
            std::cout << "FPS: " << frames << std::endl;
            frames = 0;
            millisecondCount = 0;
        }
    }

    exit();
}

void Game::exit() {
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    SDL_GameControllerClose(_controller);

    _window = nullptr;
    _renderer = nullptr;
    _controller = nullptr;

    IMG_Quit();
    SDL_Quit();
}