#ifndef SPREADER_H
#define SPREADER_H

#include "SpritesheetPropertiesComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Spreader {
    public:
        Spreader() = default;
        ~Spreader() = default;

        static Entity create();
        static Entity create(float x, float y);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        const static int NUM_OF_PAINTING_FRAMES = 4;
        const static int MS_BETWEEN_PAINTING_FRAMES = 100;

    };
}

#endif