#ifndef RENDERER_SHADOW_PASS_HPP
#define RENDERER_SHADOW_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"

#include "../../core/components.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class ShadowPass : public RenderPass {
public:
    ShadowPass(event::Dispatcher& dispatcher) : RenderPass("Shadow Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/shadow/shadow.vert");
        shader.addShader("../../../assets/shaders/shadow/shadow.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~ShadowPass() override {}

    void render(entt::registry& registry) override {
        glEnable(GL_DEPTH_TEST);
        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));
        for (auto [ent, mesh] : registry.view<std::shared_ptr<Mesh>>().each()) {
            mesh->draw(shader, {}, false);
        }
    }

    void UI() override {

    }

private:

};

} // namespace renderer

#endif