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
        ecs->addComponent<TransformComponent>(ent, TransformComponent{{x, y}, {x, y}});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::SOUTH});
        ecs->addComponent<InputComponent>(ent, InputComponent{{InputEvent::UP, InputEvent::LEFT, InputEvent::DOWN, InputEvent::RIGHT, InputEvent::ACTION, InputEvent::LOCK}});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<PlayerScript>()});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<PhysicsComponent>(ent, PhysicsComponent{{0.f, 0.f}, {1.f, 1.f}});
        ecs->addComponent<CollisionComponent>(ent, CollisionComponent{});
        ecs->addComponent<TileFlipComponent>(ent, TileFlipComponent{true});

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        render.renderQuadOffset = {0, 0};
        ecs->addComponent<RenderComponent>(ent, render);

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::PLAYER))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Player::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent propsComp;
        propsComp.spritesheet = spritesheet;

        SpritesheetProperties props = {
            0, // xTileIndex
            0, // yTileIndex
            false, // isAnimated
            false, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.setPrimarySpritesheetProperties(props);
        // props.addSpritesheetProperties(EntityState::IDLE, Direction::SOUTH, props);

        return propsComp;
    }
}