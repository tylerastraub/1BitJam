#ifndef COLLISION_COMPONENT_H
#define COLLISION_COMPONENT_H

#include "vec2.h"

#include <SDL.h>

struct CollisionComponent {
    bool collidingLeft = false;
    bool collidingRight = false;
    bool collidingUp = false;
    bool collidingDown = false;
};

#endif