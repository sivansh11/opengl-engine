#ifndef RENDERER_COMPOSITE_PASS_HPP
#define RENDERER_COMPOSITE_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"

#include "../../core/components.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class CompositePass : public RenderPass {
public:
    CompositePass(event::Dispatcher& dispatcher) : RenderPass("Composite Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/composite/composite.vert");
        shader.addShader("../../../assets/shaders/composite/composite.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~CompositePass() override {}

    void render(entt::registry& registry) override {
        renderContext->at("texAlbedoSpec").as<std::shared_ptr<gfx::Texture>>()->bind("texAlbedoSpec", 0, shader);
        renderContext->at("texEmissive").as<std::shared_ptr<gfx::Texture>>()->bind("texEmissive", 9, shader);
        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 1, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 5, shader);
        renderContext->at("texShadow").as<std::shared_ptr<gfx::Texture>>()->bind("texShadow", 3, shader);
        renderContext->at("texAmbientOcclusion").as<std::shared_ptr<gfx::Texture>>()->bind("texAmbientOcclusion", 4, shader);
        renderContext->at("texIndirectLight").as<std::shared_ptr<gfx::Texture>>()->bind("texIndirectLight", 7, shader);
    
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));

        auto& dlc = renderContext->at("directionalLight").as<core::DirectionalLightComponent>();
        shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));

        shader.vec3f("cameraPosition", glm::value_ptr(renderContext->at("cameraPosition").as<glm::vec3>()));

        renderContext->at("pointLightBuffer").as<std::shared_ptr<gfx::Buffer>>()->bind(0);
        shader.veci("numPointLights", renderContext->at("numPointLights").as<int>());

        renderContext->at("frameBufferQuadVertexAttribute").as<std::shared_ptr<gfx::VertexAttribute>>()->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {

    }

private:

};

} // namespace renderer

#endif