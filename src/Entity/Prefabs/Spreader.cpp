#include "Spreader.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
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
#include "HealthComponent.h"

#include <cmath>

namespace {
    class SpreaderScript : public IScript {
    public:
        SpreaderScript() = default;
        ~SpreaderScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& health = ecs->getComponent<HealthComponent>(owner);
            auto& state = ecs->getComponent<StateComponent>(owner);

            if(health.points < 100) {
                // we got hit - let's paint !
                health.points = 100;
                auto& painter = ecs->getComponent<PainterComponent>(owner);
                auto& transform = ecs->getComponent<TransformComponent>(owner);
                strb::vec2 pos = transform.position;
                painter.requestsPaint = true;
                painter.paintPos = {
                    pos + strb::vec2{0, 1},
                    pos + strb::vec2{0, -1},
                    pos + strb::vec2{1, 0},
                    pos + strb::vec2{1, 1},
                    pos + strb::vec2{1, -1},
                    pos + strb::vec2{-1, 0},
                    pos + strb::vec2{-1, 1},
                    pos + strb::vec2{-1, -1}
                };
                state.state = EntityState::PAINTING;
                ecs->getComponent<AnimationComponent>(owner).msSinceAnimationStart = 0;
                // audio->playAudio(owner, AudioSound::SPREADER, 0.5f);
            }
        }

    private:

    };
}

namespace prefab {
    Entity Spreader::create() {
        return create(0.f, 0.f);
    }

    Entity Spreader::create(float x, float y) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<TransformComponent>(ent, TransformComponent{{x, y}, {x, y}, {x, y}});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::SOUTH});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<SpreaderScript>()});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<PainterComponent>(ent, PainterComponent{true, TileStatus::LIGHT});
        ecs->addComponent<PaintAttackComponent>(ent, PaintAttackComponent{});
        ecs->addComponent<NavigationComponent>(ent, NavigationComponent{});
        ecs->addComponent<HealthComponent>(ent, HealthComponent{100});
        ecs->addComponent<CollisionComponent>(ent, CollisionComponent{});

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        render.renderQuadOffset = {0, 0};
        ecs->addComponent<RenderComponent>(ent, render);
        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::SPREADER))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Spreader::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent propsComp;
        propsComp.spritesheet = spritesheet;

        SpritesheetProperties idleSouth = {
            0, // xTileIndex
            0, // yTileIndex
            false, // isAnimated
            false, // isLooped
            1, // numOfFrames
            1, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::IDLE, Direction::SOUTH, idleSouth);

        SpritesheetProperties paintingSouth = {
            0, // xTileIndex
            1, // yTileIndex
            true, // isAnimated
            false, // isLooped
            NUM_OF_PAINTING_FRAMES, // numOfFrames
            MS_BETWEEN_PAINTING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::PAINTING, Direction::SOUTH, paintingSouth);

        return propsComp;
    }
}