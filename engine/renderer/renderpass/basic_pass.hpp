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
    BasicRenderPass(event::Dispatcher& dispatcher) : RenderPass("Basic Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/basic/basic.vert");
        shader.addShader("../../../assets/shaders/basic/basic.frag");
        shader.link();

        pointLigthBuffer = gfx::Buffer{gfx::Buffer::Useage::eDYNAMIC_DRAW};
    }

    ~BasicRenderPass() override {

    } 

    void render(entt::registry& registry) override {
        assert(renderContext->contains("view"));
        assert(renderContext->contains("projection"));

        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
        pointLigthBuffer.push(pointLights.data());

        glEnable(GL_DEPTH_TEST);
        shader.veci("numLights", pointLights.size());
        shader.vec3f("viewPos", glm::value_ptr(renderContext->at("viewPos").as<glm::vec3>()));
        shader.mat4f("view", glm::value_ptr(renderContext->at("view").as<glm::mat4>()));
        shader.mat4f("projection", glm::value_ptr(renderContext->at("projection").as<glm::mat4>()));
        pointLigthBuffer.bind(0);
        auto view = registry.view<Model, core::TransformComponent>();
        for (auto ent : view) {
            auto [model, transform] = registry.get<Model, core::TransformComponent>(ent);
            model.draw(shader, transform);
        }
        
    }

private:
    gfx::Buffer pointLigthBuffer;
};

} // namespace renderer

#endif