#ifndef NAVIGATION_SYSTEM_H
#define NAVIGATION_SYSTEM_H

#include "System.h"
#include "Level.h"

class NavigationSystem : public System {
public:
    NavigationSystem() = default;
    ~NavigationSystem() = default;

    void setLevel(Level* level);

    void onEntityCreated(Entity entity) override;
    void onEntityDelete(Entity entity) override;

private:
    Level* _level;

};

#endif