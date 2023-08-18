#ifndef TRACK_COMPONENT_H
#define TRACK_COMPONENT_H

#include "vec2.h"

#include <vector>

struct TrackComponent {
private:
    int _currentNode = 0;

public:
    std::vector<strb::vec2> path;

    strb::vec2 getCurrentNode() {
        return path.at(_currentNode);
    }

    void advanceNode() {
        ++_currentNode;
        if(_currentNode >= path.size()) _currentNode = 0;
    }
};

#endif