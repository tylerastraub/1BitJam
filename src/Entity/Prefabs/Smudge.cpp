#include "Smudge.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
#include "RandomGen.h"
// Components
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "DirectionComponent.h"
#include "TransformComponent.h"
#include "StateComponent.h"
#include "PhysicsComponent.h"
#include "CollisionComponent.h"
#include "AnimationComponent.h"
#include "PainterComponent.h"
#include "PaintAttackComponent.h"
#include "NavigationComponent.h"

#include <cmath>

namespace {
    class SmudgeScript : public IScript {
    public:
        SmudgeScript() {
            _actionTimerLimit = RandomGen::getRandomInt(MIN_ACTION_TIMER, MAX_ACTION_TIMER);
        }
        ~SmudgeScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& transform = ecs->getComponent<TransformComponent>(owner);
            auto& painter = ecs->getComponent<PainterComponent>(owner);
            auto& paintAttack = ecs->getComponent<PaintAttackComponent>(owner);
            auto& navigation = ecs->getComponent<NavigationComponent>(owner);
            auto& direction = ecs->getComponent<DirectionComponent>(owner);

            _actionTimerCounter += timescale * 1000;
            painter.paintPos = {std::roundf(transform.position.x), std::roundf(transform.position.y)};

            if(_actionTimerCounter >= _actionTimerLimit) {
                _actionTimerCounter = 0;
                _actionTimerLimit = RandomGen::getRandomInt(MIN_ACTION_TIMER, MAX_ACTION_TIMER);
                painter.requestsPaint = true;
                std::vector<strb::vec2> neighbors = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
                strb::vec2 goalPos = {-1, -1};
                bool foundGoal = false;
                while(!foundGoal) {
                    if(neighbors.empty()) break;
                    int i = RandomGen::getRandomInt(0, 3);
                    goalPos = transform.position + neighbors[i];
                    if(navigation.level.getTileAt(goalPos.x, goalPos.y).type == TileType::GROUND) {
                        foundGoal = true;
                        transform.goalPosition = goalPos;
                        // state change
                        if(neighbors[i] == strb::vec2{1, 0}) {
                            direction.direction = Direction::EAST;
                        }
                        else if(neighbors[i] == strb::vec2{0, 1}) {
                            direction.direction = Direction::SOUTH;
                        }
                        else if(neighbors[i] == strb::vec2{-1, 0}) {
                            direction.direction = Direction::WEST;
                        }
                        else if(neighbors[i] == strb::vec2{0, -1}) {
                            direction.direction = Direction::NORTH;
                        }
                    }
                    else {
                        int counter = 0;
                        for(auto it = neighbors.begin(); it != neighbors.end(); ++it) {
                            if(counter == i) {
                                neighbors.erase(it);
                                break;
                            }
                            ++counter;
                        }
                    }
                }
            }
        }

    private:
        const int MIN_ACTION_TIMER = 3000;
        const int MAX_ACTION_TIMER = 10000;
        int _actionTimerCounter = 0;
        int _actionTimerLimit = 0;

    };
}

namespace prefab {
    Entity Smudge::create() {
        return create(0.f, 0.f);
    }

    Entity Smudge::create(float x, float y) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<TransformComponent>(ent, TransformComponent{{x, y}, {x, y}, {x, y}});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::SOUTH});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<SmudgeScript>()});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<PhysicsComponent>(ent, PhysicsComponent{{0.f, 0.f}, {0.05f, 0.05f}});
        ecs->addComponent<CollisionComponent>(ent, CollisionComponent{});
        ecs->addComponent<PainterComponent>(ent, PainterComponent{true, TileStatus::DARK});
        ecs->addComponent<PaintAttackComponent>(ent, PaintAttackComponent{});
        ecs->addComponent<NavigationComponent>(ent, NavigationComponent{});

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        render.renderQuadOffset = {0, 0};
        ecs->addComponent<RenderComponent>(ent, render);
        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::SMUDGE))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Smudge::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent propsComp;
        propsComp.spritesheet = spritesheet;

        SpritesheetProperties idleSouth = {
            0, // xTileIndex
            0, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::IDLE, Direction::SOUTH, idleSouth);

        SpritesheetProperties idleEast = {
            0, // xTileIndex
            1, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::IDLE, Direction::EAST, idleEast);

        SpritesheetProperties idleWest = {
            0, // xTileIndex
            1, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_HORIZONTAL, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::IDLE, Direction::WEST, idleWest);

        SpritesheetProperties idleNorth = {
            0, // xTileIndex
            2, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::IDLE, Direction::NORTH, idleNorth);

        return propsComp;
    }
}