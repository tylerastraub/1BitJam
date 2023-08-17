#ifndef SPLATTER_H
#define SPLATTER_H

#include "SpritesheetPropertiesComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Smudge {
    public:
        Smudge() = default;
        ~Smudge() = default;

        static Entity create();
        static Entity create(float x, float y);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        const static int NUM_OF_IDLE_FRAMES = 3;
        const static int MS_BETWEEN_IDLE_FRAMES = 150;

    };
}

#endif