#ifndef BASE_RENDERER_HPP
#define BASE_RENDERER_HPP

#include "../core/window.hpp"

#include "../core/entity.hpp"

namespace renderer {

class BaseRenderer {
public:
    BaseRenderer(core::Window& window);
    ~BaseRenderer() = default;

    core::Window& m_window;
};

} // namespace renderer

#endif