#ifndef SCRUBBER_H
#define SCRUBBER_H

#include "SpritesheetPropertiesComponent.h"
#include "vec2.h"

#include <cstdint>
#include <vector>

using Entity = std::uint16_t;

namespace prefab {
    class Scrubber {
    public:
        Scrubber() = default;
        ~Scrubber() = default;

        static Entity create();
        static Entity create(float x, float y, std::vector<strb::vec2> path);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        const static int NUM_OF_MOVING_FRAMES = 4;
        const static int MS_BETWEEN_MOVING_FRAMES = 80;

    };
}

#endif