#ifndef RENDERER_VISULIZE_VOXELS_HPP
#define RENDERER_VISULIZE_VOXELS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/texture.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class VisualizePass : public RenderPass {
public:

    VisualizePass(event::Dispatcher& dispatcher) : RenderPass("Visualize Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/visualize.vert");
        shader.addShader("../../../assets/shaders/voxels/visualize.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~VisualizePass() override {

    } 

    void render(entt::registry& registry) override {

        if (renderContext->at("showing").as<std::string>() != "texVisualizeVoxel") return;

        shader.veci("voxelDimensions", renderContext->at("voxelDimensions").as<int>());
        shader.vecf("voxelGridSize", renderContext->at("voxelGridSize").as<float>());

        auto voxels = renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>();
        // voxels->changeMinFilter(gfx::Texture::MinFilter::eNEAREST_MIPMAP_NEAREST);
        // voxels->changeMagFilter(gfx::Texture::MagFilter::eNEAREST);

        voxels->bind("voxels", 7, shader);
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));

        shader.vec3f("cameraPosition", glm::value_ptr(renderContext->at("cameraPosition").as<glm::vec3>()));
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 1, shader);
        
        renderContext->at("frameBufferQuadVertexAttribute").as<std::shared_ptr<gfx::VertexAttribute>>()->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // voxels->changeMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_LINEAR);
        // voxels->changeMagFilter(gfx::Texture::MagFilter::eLINEAR);
    }

    void UI() override {

    }

private:
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
};

} // namespace renderer

#endif