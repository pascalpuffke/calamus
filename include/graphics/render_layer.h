#pragma once

namespace calamus {

class Renderer;

class RenderLayer {
public:
    explicit RenderLayer(Renderer& renderer)
        : renderer(renderer) { }
    virtual void on_render() = 0;

protected:
    Renderer& renderer;
};

}
