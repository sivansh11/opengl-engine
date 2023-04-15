#ifndef RENDERER_RENDERPASS_HPP
#define RENDERER_RENDERPASS_HPP

#include "pipeline.hpp"

namespace renderer {

class RenderPass {
public:
    RenderPass(const std::string& renderPassName) : m_renderPassName(renderPassName) {}
    virtual ~RenderPass() {}

    virtual void render(entt::registry& registry, RenderContext& renderContext) = 0;

friend class BasePipeline;
private:
    std::string m_renderPassName;
};

} // namespace renderer

#endif