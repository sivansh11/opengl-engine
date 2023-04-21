#ifndef RENDERER_COMPOSITE_HPP
#define RENDERER_COMPOSITE_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class CompositePass : public RenderPass {
public:
    CompositePass(event::Dispatcher& dispatcher) : RenderPass("Composite Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/composite/composite.vert");
        shader.addShader("../../../assets/shaders/composite/composite.frag");
        shader.link();

        pointLigthBuffer = gfx::Buffer{gfx::Buffer::Useage::eDYNAMIC_DRAW};

        vertexBuffer = gfx::Buffer{static_cast<uint32_t>(gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0])), gfx::Buffer::Useage::eDYNAMIC_DRAW, "PointLightBuffer"};
        vertexBuffer.push(gfx::frameBufferQuadVertices.data());
        vao.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        vao.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
        vao.bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);
    }

    ~CompositePass() override {

    } 

    void render(entt::registry& registry) override {
        assert(renderContext->contains("depthMap"));
        assert(renderContext->contains("viewPos"));
        assert(renderContext->contains("lightSpace"));
        assert(renderContext->contains("texAlbedoSpec"));
        assert(renderContext->contains("texNormal"));
        assert(renderContext->contains("texDepth"));
        assert(renderContext->contains("invView"));
        assert(renderContext->contains("invProjection"));


        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        bool hasDirectionalLight = false;
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            hasDirectionalLight = true;
            break;  
        }

        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        if (pointLights.size() > 0) {
            pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
            pointLigthBuffer.push(pointLights.data());
        } else {

        }
        // uniforms
        if (hasDirectionalLight) {
            shader.veci("hasDirectionalLight", true);
            shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
            shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
            shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
            shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));
            renderContext->at("depthMap").as<std::shared_ptr<gfx::Texture>>()->bind("depthMap", 4, shader);
        } else {
            shader.veci("hasDirectionalLight", false);
        }

        shader.veci("numLights", pointLights.size());
        shader.vec3f("viewPos", glm::value_ptr(renderContext->at("viewPos").as<glm::vec3>()));
        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        
        renderContext->at("texAlbedoSpec").as<std::shared_ptr<gfx::Texture>>()->bind("texAlbedoSpec", 0, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 1, shader);
        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 2, shader);

        if (useSSAO && renderContext->contains("ssaoImage")) {
            shader.veci("useSSAO", 1);
            renderContext->at("ssaoImage").as<std::shared_ptr<gfx::Texture>>()->bind("texSSAO", 3, shader);
        } else {
            shader.veci("useSSAO", 0);
        }
        // ssbo
        pointLigthBuffer.bind(0);

        // draw call
        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {
        ImGui::Checkbox("use SSAO", &useSSAO);
    }

private:
    gfx::Buffer pointLigthBuffer;
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
    bool useSSAO = true;
};

} // namespace renderer

#endif