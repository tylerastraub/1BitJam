#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "System.h"

class PhysicsSystem : public System {
public:
    PhysicsSystem() = default;
    ~PhysicsSystem() = default;

    void update(float timescale);

private:
    void moveToZero(float &value, float amount);

};

#endif