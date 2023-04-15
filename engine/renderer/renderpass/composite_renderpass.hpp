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

        vertexBuffer = gfx::Buffer{gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0])};
        vertexBuffer.push(gfx::frameBufferQuadVertices.data());
        vao.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        vao.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
        vao.bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);
    }

    ~CompositePass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
        pointLigthBuffer.push(pointLights.data());

        glEnable(GL_DEPTH_TEST);
        shader.bind();
        shader.veci("numLights", pointLights.size());
        shader.vec3f("viewPos", glm::value_ptr(std::any_cast<glm::vec3>(renderContext["viewPos"])));
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texAlbedoSpec"])->bind("texAlbedoSpec", 0, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texPosition"])->bind("texPosition", 1, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texNormal"])->bind("texNormal", 2, shader);
        pointLigthBuffer.bind(0);

        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    gfx::ShaderProgram shader;
    gfx::Buffer pointLigthBuffer;
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
};

} // namespace renderer

#endif