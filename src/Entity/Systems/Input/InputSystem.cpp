#include "InputSystem.h"
#include "EntityRegistry.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "TileFlipComponent.h"
#include "DirectionComponent.h"
#include "TransformComponent.h"

#include <algorithm>

void InputSystem::init(Keyboard* keyboard, Controller* controller, Settings* settings) {
    _keyboard = keyboard;
    _controller = controller;
    _settings = settings;
}

void InputSystem::update() {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto& inputComponent = ecs->getComponent<InputComponent>(ent);
        auto& physics = ecs->getComponent<PhysicsComponent>(ent);
        auto& dir = ecs->getComponent<DirectionComponent>(ent);
        auto& tileFlip = ecs->getComponent<TileFlipComponent>(ent);
        auto allowedInputs = inputComponent.allowedInputs;

        // Lock input
        if(inputDown(InputEvent::LOCK) &&
            std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::LOCK) != allowedInputs.end()) {
            tileFlip.isLocked = true;
        }
        else {
            tileFlip.isLocked = false;
        }

        // X inputs
        if(inputPressed(InputEvent::LEFT) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::LEFT) != allowedInputs.end()) {
            _inputRequested = true;
            if(!tileFlip.isLocked) physics.velocity.x = physics.moveSpeed.x * -1;
            dir.direction = Direction::WEST;
        }
        else if(inputPressed(InputEvent::RIGHT) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::RIGHT) != allowedInputs.end()) {
            _inputRequested = true;
            if(!tileFlip.isLocked) physics.velocity.x = physics.moveSpeed.x;
            dir.direction = Direction::EAST;
        }

        // Y inputs
        if(inputPressed(InputEvent::UP) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::UP) != allowedInputs.end()) {
            _inputRequested = true;
            if(!tileFlip.isLocked) physics.velocity.y = physics.moveSpeed.y * -1;
            dir.direction = Direction::NORTH;
        }
        else if(inputPressed(InputEvent::DOWN) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::DOWN) != allowedInputs.end()) {
            _inputRequested = true;
            if(!tileFlip.isLocked) physics.velocity.y = physics.moveSpeed.y;
            dir.direction = Direction::SOUTH;
        }

        // Other inputs
        if(inputPressed(InputEvent::ACTION) &&
           std::find(allowedInputs.begin(), allowedInputs.end(), InputEvent::ACTION) != allowedInputs.end() &&
           ecs->hasComponent<TileFlipComponent>(ent) &&
           ecs->hasComponent<DirectionComponent>(ent)) {
            _inputRequested = true;
            auto& transform = ecs->getComponent<TransformComponent>(ent);
            if(tileFlip.canFlipTiles) {
                tileFlip.isFlippingTile = true;
                strb::vec2 offset = {0.f, 0.f};
                switch(dir.direction) {
                    case Direction::NORTH:
                        offset.y -= 1;
                        break;
                    case Direction::SOUTH:
                        offset.y += 1;
                        break;
                    case Direction::WEST:
                        offset.x -= 1;
                        break;
                    case Direction::EAST:
                        offset.x += 1;
                        break;
                }
                tileFlip.tileFlipPos = transform.position + offset;
            }
        }
    }

    _keyboard->updateInputs();
    _controller->updateInputs();
}

void InputSystem::completeInputRequest() {
    _inputRequested = false;
}

bool InputSystem::requestsInput() {
    return _inputRequested;
}

bool InputSystem::inputDown(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return _controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1 ||
            _controller->isButtonDown(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            _keyboard->isKeyDown(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonDown(_settings->getButton(input)) || _keyboard->isKeyDown(_settings->getScancode(input));
    }
}

bool InputSystem::inputUp(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return !(_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1 ||
            _controller->isButtonUp(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN)) ||
            _keyboard->isKeyUp(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonUp(_settings->getButton(input)) || _keyboard->isKeyUp(_settings->getScancode(input));
    }
}

bool InputSystem::inputPressed(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != -1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != -1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 1) ||
            _controller->isButtonPressed(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            _keyboard->isKeyPressed(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonPressed(_settings->getButton(input)) || _keyboard->isKeyPressed(_settings->getScancode(input));
    }
}

bool InputSystem::inputReleased(InputEvent input) {
    if(input == InputEvent::LEFT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == -1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::LEFT));
    }
    else if(input == InputEvent::RIGHT) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) == 1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::RIGHT));
    }
    else if(input == InputEvent::UP) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != -1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == -1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::UP));
    }
    else if(input == InputEvent::DOWN) {
        return (_controller->getAxisState(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 1 &&
            _controller->getAxisStateLastTick(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) == 1) ||
            _controller->isButtonReleased(SDL_GameControllerButton_Extended::SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            _keyboard->isKeyReleased(_settings->getScancode(InputEvent::DOWN));
    }
    else {
        return _controller->isButtonReleased(_settings->getButton(input)) || _keyboard->isKeyReleased(_settings->getScancode(input));
    }
}