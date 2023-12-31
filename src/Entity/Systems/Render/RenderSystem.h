#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "System.h"
#include "vec2.h"

#include <SDL.h>

class RenderSystem : public System {
public:
    RenderSystem() = default;
    ~RenderSystem() = default;

    void update(float timescale);

    void render(SDL_Renderer* renderer, int renderXOffset = 0, int renderYOffset = 0);

    void setRenderBounds(strb::vec2 renderBounds);

private:
    const int RENDER_TILE_SIZE = 16;

    strb::vec2 _renderBounds = {0, 0};

};

#endif