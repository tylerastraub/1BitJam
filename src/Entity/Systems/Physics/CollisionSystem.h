#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "System.h"
#include "Level.h"

class CollisionSystem : public System {
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;
    
    void update(float timescale, Level* level);

private:

};

#endif