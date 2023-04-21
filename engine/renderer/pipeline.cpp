#include "pipeline.hpp"

#include "renderpass.hpp"

#include "../core/imgui_utils.hpp"

#include <glad/glad.h>

#include <iostream>

namespace renderer {

void BasePipeline::addRenderPass(std::shared_ptr<RenderPass> renderPass) {
    m_renderPasses.push_back(renderPass);
}

void BasePipeline::render(entt::registry& registry, RenderContext& renderContext) {
    BasePipeline::renderContext = &renderContext;
    timer.begin();
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_name.c_str());
    preRender(registry);
    for (int i = 0; i < m_renderPasses.size(); i++) {
        auto& renderPass = m_renderPasses[i];
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, renderPass->m_name.c_str());
        // timer.begin();
        renderPass->shader.bind();
        renderPass->renderContext = &renderContext;
        renderPass->render(registry);
        // timer.end();
        // if (auto time = timer.popTimeStamp()) {
        //     renderContext.at(renderPass->m_name) = time.value();
        // } else {
        //     renderContext.at(renderPass->m_name) = 0;
        // }
        glPopDebugGroup();
    }
    postRender(registry);
    glPopDebugGroup();
    timer.end();
}

void BasePipeline::UI() {
    pipelineUI();
    if (auto time = timer.popTimeStamp()) {
        ImGui::Text("%s took %fms", m_name.c_str(), float(time.value() / 1000000.0));
    } else {
        ImGui::Text("Timer information not availble!");
    }
    ImGui::Separator();
    for (auto renderPass : m_renderPasses) {
        ImGui::Text("%s", renderPass->m_name.c_str());
        renderPass->UI();
        ImGui::Separator();
    }
}

} // namespace renderer
