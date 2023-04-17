#ifndef RENDERER_BASIC_HPP
#define RENDERER_BASIC_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class BasicRenderPass : public RenderPass {
public:
    BasicRenderPass() : RenderPass("Basic RenderPass") {
        shader.addShader("../../../assets/shaders/basic/basic.vert");
        shader.addShader("../../../assets/shaders/basic/basic.frag");
        shader.link();

        pointLigthBuffer = gfx::Buffer{gfx::Buffer::Useage::eDYNAMIC_DRAW};
    }

    ~BasicRenderPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        assert(renderContext.contains("view"));
        assert(renderContext.contains("projection"));

        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
        pointLigthBuffer.push(pointLights.data());

        glEnable(GL_DEPTH_TEST);
        shader.veci("numLights", pointLights.size());
        shader.vec3f("viewPos", glm::value_ptr(std::any_cast<glm::vec3>(renderContext["viewPos"])));
        shader.mat4f("view", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["view"])));
        shader.mat4f("projection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["projection"])));
        pointLigthBuffer.bind(0);
        auto view = registry.view<Model>();
        for (auto ent : view) {
            auto& model = registry.get<Model>(ent);
            model.draw(shader, {});
        }
        
    }

private:
    gfx::Buffer pointLigthBuffer;
};

} // namespace renderer

#endif