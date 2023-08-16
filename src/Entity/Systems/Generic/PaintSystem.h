#ifndef PAINT_SYSTEM_H
#define PAINT_SYSTEM_H

#include "System.h"
#include "Level.h"

class PaintSystem : public System {
public:
    PaintSystem() = default;
    ~PaintSystem() = default;

    void update(Level* level);

private:

};

#endif