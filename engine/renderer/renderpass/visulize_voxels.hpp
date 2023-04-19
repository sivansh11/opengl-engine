#ifndef RENDERER_VISULIZE_VOXELS_HPP
#define RENDERER_VISULIZE_VOXELS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/texture.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class VisulizationPass : public RenderPass {
public:

    VisulizationPass(event::Dispatcher& dispatcher) : RenderPass("Visulization Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/visualize.vert");
        shader.addShader("../../../assets/shaders/voxels/visualize.geom");
        shader.addShader("../../../assets/shaders/voxels/visualize.frag");
        shader.link();

    }

    ~VisulizationPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        if (std::any_cast<std::string>(renderContext["showing"]) != "voxelVisual") return;
        shader.veci("voxelDim", std::any_cast<int>(renderContext["voxelDim"]));
        shader.vecf("voxelGridSize", std::any_cast<float>(renderContext["voxelGridSize"]));
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["voxels"])->bind("voxels", 7, shader);
        shader.mat4f("view", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["view"])));
        shader.mat4f("projection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["projection"])));
        shader.mat4f("model", glm::value_ptr(glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(std::any_cast<float>(renderContext["voxelGridSize"]) / (float)std::any_cast<int>(renderContext["voxelDim"]))), glm::vec3{0, 0, 0})));

        vao.bind();
        glDrawArrays(GL_POINTS, 0, std::any_cast<int>(renderContext["voxelDim"]) * std::any_cast<int>(renderContext["voxelDim"]) * std::any_cast<int>(renderContext["voxelDim"]));
    }

    void UI() override {

    }

private:
    gfx::VertexAttribute vao;
};

} // namespace renderer

#endif