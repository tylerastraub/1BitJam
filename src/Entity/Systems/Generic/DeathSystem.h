#ifndef DEATH_SYSTEM_H
#define DEATH_SYSTEM_H

#include "System.h"

#include <unordered_map>

class DeathSystem : public System {
public:
    DeathSystem() = default;
    ~DeathSystem() = default;

    void update(float timescale);

    int getNumOfDeaths();

private:
    int _despawnTime = 1000;
    std::unordered_map<Entity, int> _entityTimers;
    int _deaths = 0;

};

#endif