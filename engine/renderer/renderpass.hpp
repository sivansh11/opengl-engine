#ifndef RENDERER_RENDERPASS_HPP
#define RENDERER_RENDERPASS_HPP

#include "pipeline.hpp"
#include "../gfx/shaders.hpp"

namespace renderer {

class RenderPass : public core::BasePanel {
public:
    RenderPass(const std::string& renderPassName) : BasePanel(renderPassName) {}
    virtual ~RenderPass() {}

    virtual void render(entt::registry& registry, RenderContext& renderContext) = 0;

protected:
    gfx::ShaderProgram shader;

friend class BasePipeline;
private:

};

} // namespace renderer

#endif