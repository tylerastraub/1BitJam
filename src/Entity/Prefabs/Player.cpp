#include "Player.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "PlayerComponent.h"
#include "InputComponent.h"
#include "RenderComponent.h"
#include "ScriptComponent.h"
#include "DirectionComponent.h"
#include "TransformComponent.h"
#include "StateComponent.h"
#include "PhysicsComponent.h"
#include "CollisionComponent.h"
#include "TileFlipComponent.h"
#include "AnimationComponent.h"
#include "PainterComponent.h"
#include "PaintAttackComponent.h"
#include "PowerupComponent.h"

#include <cmath>

namespace {
    class PlayerScript : public IScript {
    public:
        PlayerScript() = default;
        ~PlayerScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& transform = ecs->getComponent<TransformComponent>(owner);
            auto& input = ecs->getComponent<InputComponent>(owner);
            auto& painter = ecs->getComponent<PainterComponent>(owner);
            auto& paintAttack = ecs->getComponent<PaintAttackComponent>(owner);
            auto& state = ecs->getComponent<StateComponent>(owner);
            auto& physics = ecs->getComponent<PhysicsComponent>(owner);
            auto powerup = ecs->getComponent<PowerupComponent>(owner);
            painter.requestsPaint = true;
            painter.paintPos.push_back({std::roundf(transform.position.x), std::roundf(transform.position.y)});

            if(paintAttack.msSinceLastPaintAttack < paintAttack.msCantActAfterPaintAttack) {
                input.allowedInputs = {};
            }
            else if(transform.position == transform.goalPosition && physics.velocity.x == 0.f && physics.velocity.y == 0.f) {
                input.allowedInputs = {InputEvent::UP, InputEvent::LEFT, InputEvent::DOWN, InputEvent::RIGHT, InputEvent::LOCK};
                if(powerup.paintAttack) input.allowedInputs.push_back(InputEvent::ACTION);
            }
            else {
                input.allowedInputs = {InputEvent::UP, InputEvent::LEFT, InputEvent::DOWN, InputEvent::RIGHT};
            }

            // state checking
            if(paintAttack.msSinceLastPaintAttack < paintAttack.msCantActAfterPaintAttack) {
                state.state = EntityState::PAINTING;
            }
            else if(physics.velocity.x != 0.f || physics.velocity.y != 0.f) {
                state.state = EntityState::MOVING;
            }
            else {
                state.state = EntityState::IDLE;
            }

            paintAttack.wasAttackingLastTick = paintAttack.attacking;
        }

    private:

    };
}

namespace prefab {
    Entity Player::create() {
        return create(0.f, 0.f);
    }

    Entity Player::create(float x, float y) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<PlayerComponent>(ent, PlayerComponent{});
        ecs->addComponent<TransformComponent>(ent, TransformComponent{{x, y}, {x, y}, {x, y}});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::SOUTH});
        ecs->addComponent<InputComponent>(ent, InputComponent{{InputEvent::UP, InputEvent::LEFT, InputEvent::DOWN, InputEvent::RIGHT, InputEvent::ACTION, InputEvent::LOCK}});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<PlayerScript>()});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<PhysicsComponent>(ent, PhysicsComponent{{0.f, 0.f}, {0.045f, 0.045f}});
        ecs->addComponent<CollisionComponent>(ent, CollisionComponent{});
        ecs->addComponent<TileFlipComponent>(ent, TileFlipComponent{false}); // disable flipping for now but can still use lock
        ecs->addComponent<PainterComponent>(ent, PainterComponent{true, TileStatus::LIGHT});
        ecs->addComponent<PaintAttackComponent>(ent, PaintAttackComponent{});

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        render.renderQuadOffset = {0, 0};
        ecs->addComponent<RenderComponent>(ent, render);
        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::PLAYER))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Player::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
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

        SpritesheetProperties movingSouth = {
            0, // xTileIndex
            3, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_SOUTH_EAST_OR_WEST_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::SOUTH, movingSouth);

        SpritesheetProperties movingEast = {
            0, // xTileIndex
            4, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_SOUTH_EAST_OR_WEST_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::EAST, movingEast);

        SpritesheetProperties movingWest = {
            0, // xTileIndex
            4, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_SOUTH_EAST_OR_WEST_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_HORIZONTAL, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::WEST, movingWest);

        SpritesheetProperties movingNorth = {
            0, // xTileIndex
            5, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_NORTH_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::NORTH, movingNorth);

        SpritesheetProperties paintingSouth = {
            0, // xTileIndex
            6, // yTileIndex
            true, // isAnimated
            false, // isLooped
            NUM_OF_PAINTING_FRAMES, // numOfFrames
            MS_BETWEEN_PAINTING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::PAINTING, Direction::SOUTH, paintingSouth);

        SpritesheetProperties paintingEast = {
            0, // xTileIndex
            7, // yTileIndex
            true, // isAnimated
            false, // isLooped
            NUM_OF_PAINTING_FRAMES, // numOfFrames
            MS_BETWEEN_PAINTING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::PAINTING, Direction::EAST, paintingEast);

        SpritesheetProperties paintingWest = {
            0, // xTileIndex
            7, // yTileIndex
            true, // isAnimated
            false, // isLooped
            NUM_OF_PAINTING_FRAMES, // numOfFrames
            MS_BETWEEN_PAINTING_FRAMES, // msBetweenFrames
            SDL_FLIP_HORIZONTAL, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::PAINTING, Direction::WEST, paintingWest);

        SpritesheetProperties paintingNorth = {
            0, // xTileIndex
            8, // yTileIndex
            true, // isAnimated
            false, // isLooped
            NUM_OF_PAINTING_FRAMES, // numOfFrames
            MS_BETWEEN_PAINTING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::PAINTING, Direction::NORTH, paintingNorth);

        return propsComp;
    }
}