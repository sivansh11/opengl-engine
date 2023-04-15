#include "pipeline.hpp"

#include "renderpass.hpp"

#include <glad/glad.h>

#include <iostream>

namespace renderer {

void BasePipeline::addRenderPass(std::shared_ptr<RenderPass> renderPass) {
    m_renderPasses.push_back(renderPass);

    // GLuint query;
    // glGenQueries(1, &query);
    // m_queries.push_back(query);
    
    // m_timeElapsed.push_back(0);
}

void BasePipeline::render(entt::registry& registry, RenderContext& renderContext) {
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_pipelineName.c_str());
    preRender(registry, renderContext);
    for (int i = 0; i < m_renderPasses.size(); i++) {
        auto& renderPass = m_renderPasses[i];
        // auto& query = m_queries[i];
        // auto& timeElapsed = m_timeElapsed[i];

        // glBeginQuery(GL_TIME_ELAPSED, query);
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, renderPass->m_renderPassName.c_str());
        renderPass->render(registry, renderContext);
        glPopDebugGroup();
        // glEndQuery(GL_TIME_ELAPSED);

        // int done = 0;
        // glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);
        // if (done)
        //     glGetQueryObjecti64v(query, GL_QUERY_RESULT, &timeElapsed);
    }
    postRender(registry, renderContext);
    glPopDebugGroup();

    // std::cout << m_pipelineName << '\n';
    // for (int i = 0; i < m_renderPasses.size(); i++) {
    //     auto& renderPass = m_renderPasses[i];
    //     auto& timeElapsed = m_timeElapsed[i];
    //     std::cout << 't' << renderPass->m_renderPassName << ": " << timeElapsed / 1000000.f << "ms" << '\n';
    // }
}

} // namespace renderer
