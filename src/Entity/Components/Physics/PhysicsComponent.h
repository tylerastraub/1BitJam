#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "vec2.h"

struct PhysicsComponent {
    strb::vec2 velocity = {0.f, 0.f};
    strb::vec2 moveSpeed = {0.f, 0.f};
};

#endif