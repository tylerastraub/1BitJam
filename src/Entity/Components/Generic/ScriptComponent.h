#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "Audio.h"

#include <memory>
#include <cstdint>

using Entity = std::uint16_t;

class IScript {
public:
    IScript() = default;
    virtual ~IScript() = default;

    /**
     * @brief Update the script.
     * 
     * @param timescale The timescale to update by - is usually fixed.
     */
    virtual void update(Entity owner, float timescale, Audio* audio) = 0;

private:

};

struct ScriptComponent {
    std::shared_ptr<IScript> script = nullptr;
};

#endif