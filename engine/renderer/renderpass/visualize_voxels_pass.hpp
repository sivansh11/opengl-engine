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

    void render(entt::registry& registry, RenderContext& renderContext) override {
        if (std::any_cast<std::string>(renderContext["showing"]) != "voxelVisual") return;
        renderContext["maxDist"] = 1000.f;
        renderContext["maxCount"] = 1000;
        renderContext["alphaThresh"] = 0.99f;
        renderContext["tanHalfAngles"] = 0.f;

        shader.veci("voxelDim", std::any_cast<int>(renderContext["voxelDim"]));
        shader.vecf("voxelGridSize", std::any_cast<float>(renderContext["voxelGridSize"]));
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["voxels"])->bind("voxels", 7, shader);
        shader.mat4f("invView", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["invView"])));
        shader.mat4f("invProjection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["invProjection"])));
        shader.veci("MAX_COUNT", std::any_cast<int>(renderContext["maxCount"]));
        shader.vecf("MAX_DIST", std::any_cast<float>(renderContext["maxDist"]));
        shader.vecf("tanHalfAngle", std::any_cast<float>(renderContext["tanHalfAngles"]));
        shader.vecf("ALPHA_THRESH", std::any_cast<float>(renderContext["alphaThresh"]));


        shader.vec3f("viewPos", glm::value_ptr(std::any_cast<glm::vec3>(renderContext["viewPos"])));
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texDepth"])->bind("texDepth", 1, shader);

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