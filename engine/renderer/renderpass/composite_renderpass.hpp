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
    CompositePass() : RenderPass("Composite Pass") {
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

    void render(entt::registry& registry, RenderContext& renderContext) override {
        assert(renderContext.contains("depthMap"));
        assert(renderContext.contains("viewPos"));
        assert(renderContext.contains("lightSpaceMatrix"));
        assert(renderContext.contains("texAlbedoSpec"));
        // assert(renderContext.contains("texPosition"));
        assert(renderContext.contains("texNormal"));
        assert(renderContext.contains("ssaoImage"));

        bool has = false;
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            has = true;
            break;  
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        if (has) {
            shader.veci("hasDirectionalLight", true);
            std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["depthMap"])->bind("depthMap", 3, shader);
            shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
            shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
            shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
            shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));
        } else {
            shader.veci("hasDirectionalLight", false);
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

        // glEnable(GL_DEPTH_TEST);
        shader.veci("numLights", pointLights.size());
        shader.vec3f("viewPos", glm::value_ptr(std::any_cast<glm::vec3>(renderContext["viewPos"])));
        shader.mat4f("lightSpaceMatrix", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["lightSpaceMatrix"])));
        shader.mat4f("invView", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["invView"])));
        shader.mat4f("invProjection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["invProjection"])));
        
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texAlbedoSpec"])->bind("texAlbedoSpec", 0, shader);
        // std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texPosition"])->bind("texPosition", 1, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texDepth"])->bind("texDepth", 1, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texNormal"])->bind("texNormal", 2, shader);
        if (useSSAO) {
            std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["ssaoImage"])->bind("texSSAO", 3, shader);
            shader.veci("useSSAO", 1);
        } else {
            shader.veci("useSSAO", 0);
        }

        if (useVXGI) {
            shader.veci("useVXGI", useVXGI);
            std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["voxels"])->bind("voxels", 4, shader);
        } else {
            shader.veci("useVXGI", useVXGI);
        }

        pointLigthBuffer.bind(0);

        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {
        ImGui::Checkbox("use SSAO", &useSSAO);
        ImGui::Checkbox("use VXGI", &useVXGI);
    }

private:
    gfx::Buffer pointLigthBuffer;
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
    bool useSSAO = true;
    bool useVXGI = true;
};

} // namespace renderer

#endif