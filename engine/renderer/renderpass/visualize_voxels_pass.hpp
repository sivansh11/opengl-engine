#ifndef RENDERER_VISULIZE_VOXELS_HPP
#define RENDERER_VISULIZE_VOXELS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/texture.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class VisualizePass : public RenderPass {
public:

    VisualizePass(event::Dispatcher& dispatcher) : RenderPass("Visualize Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/visualize.vert");
        shader.addShader("../../../assets/shaders/voxels/visualize.frag");
        shader.link();

        vertexBuffer = gfx::Buffer{static_cast<uint32_t>(gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0])), gfx::Buffer::Useage::eDYNAMIC_DRAW, "PointLightBuffer"};
        vertexBuffer.push(gfx::frameBufferQuadVertices.data());
        vao.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        vao.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
        vao.bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);
    }

    ~VisualizePass() override {

    } 

    void render(entt::registry& registry) override {
        if (renderContext->at("showing").as<std::string>() != "voxelVisual") return;
        renderContext->at("maxDist") = 1000.f;
        renderContext->at("maxCount") = 1000;
        renderContext->at("alphaThresh") = 0.99f;
        renderContext->at("tanHalfAngles") = 0.f;

        shader.veci("voxelDimensions", renderContext->at("voxelDimensions").as<int>());
        shader.vecf("voxelGridSize", renderContext->at("voxelGridSize").as<float>());
        renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>()->bind("voxels", 7, shader);
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        shader.veci("MAX_COUNT", renderContext->at("maxCount").as<int>());
        shader.vecf("MAX_DIST", renderContext->at("maxDist").as<float>());
        shader.vecf("tanHalfAngle", renderContext->at("tanHalfAngles").as<float>());
        shader.vecf("ALPHA_THRESH", renderContext->at("alphaThresh").as<float>());


        shader.vec3f("viewPos", glm::value_ptr(renderContext->at("viewPos").as<glm::vec3>()));
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 1, shader);

        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {

    }

private:
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
};

} // namespace renderer

#endif