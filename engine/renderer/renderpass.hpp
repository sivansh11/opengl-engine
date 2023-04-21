#ifndef RENDERER_RENDERPASS_HPP
#define RENDERER_RENDERPASS_HPP

#include "pipeline.hpp"
#include "../gfx/shaders.hpp"

namespace renderer {

class RenderPass : public core::BasePanel {
public:
    RenderPass(const std::string& renderPassName, event::Dispatcher& dispatcher) : BasePanel(renderPassName), dispatcher(dispatcher) {}
    virtual ~RenderPass() {}

    virtual void render(entt::registry& registry) = 0;

protected:
    gfx::ShaderProgram shader;
    RenderContext *renderContext;
    event::Dispatcher& dispatcher;

friend class BasePipeline;
private:

};

} // namespace renderer

#endif