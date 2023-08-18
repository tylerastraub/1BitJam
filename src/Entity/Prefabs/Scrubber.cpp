#include "Scrubber.h"
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
#include "HealthComponent.h"
#include "TrackComponent.h"

#include <cmath>

namespace {
    class ScrubberScript : public IScript {
    public:
        ScrubberScript(Entity owner) {
            auto ecs = EntityRegistry::getInstance();
            auto& direction = ecs->getComponent<DirectionComponent>(owner);
            Direction directions[4] = {Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST};
            direction.direction = directions[RandomGen::getRandomInt(0, 3)];
        }
        ~ScrubberScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            
            auto& health = ecs->getComponent<HealthComponent>(owner);
            if(health.points < 1) return;

            auto& transform = ecs->getComponent<TransformComponent>(owner);
            auto& painter = ecs->getComponent<PainterComponent>(owner);
            auto& navigation = ecs->getComponent<NavigationComponent>(owner);
            auto& direction = ecs->getComponent<DirectionComponent>(owner);
            auto& collision = ecs->getComponent<CollisionComponent>(owner);
            auto& track = ecs->getComponent<TrackComponent>(owner);
            
            painter.paintPos = {std::roundf(transform.position.x), std::roundf(transform.position.y)};
            painter.requestsPaint = true;
            if(transform.goalPosition == transform.position) {
                if(transform.position == track.getCurrentNode()) {
                    track.advanceNode();
                    strb::vec2 posDiff = track.getCurrentNode() - transform.position;
                    if(posDiff.x > 0) direction.direction = Direction::EAST;
                    else if(posDiff.x < 0) direction.direction = Direction::WEST;
                    else if(posDiff.y > 0) direction.direction = Direction::SOUTH;
                    else if(posDiff.y < 0) direction.direction = Direction::NORTH;
                    else direction.direction = Direction::NORTHEAST;
                }
                switch (direction.direction) {
                    case Direction::NORTH:
                        transform.goalPosition = transform.position + strb::vec2{0, -1};
                        break;
                    case Direction::SOUTH:
                        transform.goalPosition = transform.position + strb::vec2{0, 1};
                        break;
                    case Direction::EAST:
                        transform.goalPosition = transform.position + strb::vec2{1, 0};
                        break;
                    case Direction::WEST:
                        transform.goalPosition = transform.position + strb::vec2{-1, 0};
                        break;
                    default:
                        transform.goalPosition = transform.position;
                        break;
                }
            }
        }

    private:

    };
}

namespace prefab {
    Entity Scrubber::create() {
        return create(0.f, 0.f, {});
    }

    Entity Scrubber::create(float x, float y, std::vector<strb::vec2> path) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<TransformComponent>(ent, TransformComponent{{x, y}, {x, y}, {x, y}});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::SOUTH});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<ScrubberScript>(ent)});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::MOVING});
        ecs->addComponent<PhysicsComponent>(ent, PhysicsComponent{{0.f, 0.f}, {0.06f, 0.06f}});
        ecs->addComponent<CollisionComponent>(ent, CollisionComponent{});
        ecs->addComponent<PainterComponent>(ent, PainterComponent{true, TileStatus::DARK});
        ecs->addComponent<PaintAttackComponent>(ent, PaintAttackComponent{});
        ecs->addComponent<NavigationComponent>(ent, NavigationComponent{});
        ecs->addComponent<HealthComponent>(ent, HealthComponent{1});

        TrackComponent track;
        track.path = path;
        ecs->addComponent<TrackComponent>(ent, track);

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        render.renderQuadOffset = {0, 0};
        ecs->addComponent<RenderComponent>(ent, render);
        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::SCRUBBER))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Scrubber::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent propsComp;
        propsComp.spritesheet = spritesheet;

        SpritesheetProperties movingSouth = {
            0, // xTileIndex
            0, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::SOUTH, movingSouth);

        SpritesheetProperties movingEast = {
            0, // xTileIndex
            1, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::EAST, movingEast);

        SpritesheetProperties movingWest = {
            0, // xTileIndex
            1, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_HORIZONTAL, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::WEST, movingWest);

        SpritesheetProperties movingNorth = {
            0, // xTileIndex
            2, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_MOVING_FRAMES, // numOfFrames
            MS_BETWEEN_MOVING_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::MOVING, Direction::NORTH, movingNorth);

        SpritesheetProperties deadSouth = {
            0, // xTileIndex
            3, // yTileIndex
            false, // isAnimated
            false, // isLooped
            1, // numOfFrames
            1, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        propsComp.addSpritesheetProperties(EntityState::DEAD, Direction::SOUTH, deadSouth);

        return propsComp;
    }
}