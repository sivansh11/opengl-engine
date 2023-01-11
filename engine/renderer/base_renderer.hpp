#ifndef BASE_RENDERER_HPP
#define BASE_RENDERER_HPP

#include "../core/window.hpp"

#include "../core/entity.hpp"
#include "../core/components.hpp"

namespace renderer {

class BaseRenderer {
public:
    BaseRenderer(core::Window& window);
    ~BaseRenderer() = default;

    virtual void render(std::shared_ptr<core::Scene> scene, core::CameraComponent& camera) = 0;

    core::Window& m_window;
};

} // namespace renderer

#endif