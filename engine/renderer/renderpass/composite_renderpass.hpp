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
        bool has = false;
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            has = true;
            break;  
        }
        // if (has) {
        //     shader.veci("hasDepthMap", true);
        //     std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["depthImage"])->bind("depthMap", 3, shader);
        //     shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        //     shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        //     shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        //     shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));
        // }
        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
        pointLigthBuffer.push(pointLights.data());

        // glEnable(GL_DEPTH_TEST);
        shader.veci("numLights", pointLights.size());
        shader.vec3f("viewPos", glm::value_ptr(std::any_cast<glm::vec3>(renderContext["viewPos"])));
        shader.mat4f("view", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["view"])));
        shader.mat4f("projection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["projection"])));

        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texAlbedoSpec"])->bind("texAlbedoSpec", 0, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texPosition"])->bind("texPosition", 1, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texNormal"])->bind("texNormal", 2, shader);
        pointLigthBuffer.bind(0);

        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    gfx::Buffer pointLigthBuffer;
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
};

} // namespace renderer

#endif