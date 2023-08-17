#ifndef STATE_COMPONENT_H
#define STATE_COMPONENT_H

enum class EntityState {
    NOVAL = -1,
    IDLE,
    MOVING,
    PAINTING,
    DEAD,
};

struct StateComponent {
    EntityState state = EntityState::NOVAL;
};

#endif