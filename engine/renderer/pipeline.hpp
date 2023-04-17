#ifndef RENDERER_PIPELINE_HPP
#define RENDERER_PIPELINE_HPP

#include "../core/event.hpp"
#include "../core/panel.hpp"

#include <entt/entt.hpp>

#include <glad/glad.h>

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <any>

namespace renderer {

using RenderContext = std::map<std::string, std::any>;

class RenderPass;

class BasePipeline : public core::BasePanel {
public:
    BasePipeline(event::Dispatcher& dispatcher, const std::string& pipelineName) : m_dispatcher(dispatcher), BasePanel(pipelineName) {}
    virtual ~BasePipeline() {}

    void addRenderPass(std::shared_ptr<RenderPass> renderPass);
    void render(entt::registry& registry, RenderContext &renderContext);
    void UI() override;

    virtual void preRender(entt::registry& registry, RenderContext &renderContext) = 0;
    virtual void postRender(entt::registry& registry, RenderContext &renderContext) = 0;
    virtual void pipelineUI() {}

protected:
    event::Dispatcher& m_dispatcher;
    RenderContext *m_renderContextPtr;

private:
    std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
    // std::vector<GLuint> m_queries;
    // std::vector<GLint64> m_timeElapsed;
};

} // namespace renderer

#endif