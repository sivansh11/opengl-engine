#ifndef BASE_RENDERER_HPP
#define BASE_RENDERER_HPP

#include "../gfx/shaders.hpp"
#include "../gfx/framebuffer.hpp"
#include "../gfx/buffer.hpp"
#include "../gfx/vertex_attribute.hpp"

namespace renderer {

class BaseRenderer {
public:
    BaseRenderer() = default;
    ~BaseRenderer() = default;

    void render();

private:

};

} // namespace renderer

#endif