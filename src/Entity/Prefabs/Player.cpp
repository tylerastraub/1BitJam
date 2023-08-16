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

namespace {
    class PlayerScript : public IScript {
    public:
        PlayerScript() = default;
        ~PlayerScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
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
        ecs->addComponent<PhysicsComponent>(ent, PhysicsComponent{{0.f, 0.f}, {0.035f, 0.035f}});
        ecs->addComponent<CollisionComponent>(ent, CollisionComponent{});
        ecs->addComponent<TileFlipComponent>(ent, TileFlipComponent{true});

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

        return propsComp;
    }
}