#ifndef PLAYER_H
#define PLAYER_H

#include <cstdint>

#include "SpritesheetPropertiesComponent.h"

using Entity = std::uint16_t;

namespace prefab {
    class Player {
    public:
        Player() = default;
        ~Player() = default;

        static Entity create();
        static Entity create(float x, float y);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        const static int NUM_OF_IDLE_FRAMES = 1;
        const static int MS_BETWEEN_IDLE_FRAMES = 1;
    };
}

#endif