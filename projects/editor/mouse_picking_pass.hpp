#ifndef MOUSE_PICKING_PASS_HPP
#define MOUSE_PICKING_PASS_HPP

#include "renderer/renderpass.hpp"
#include "renderer/model.hpp"

#include "gfx/shaders.hpp"
#include "gfx/buffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "core/events.hpp"

class MousePickingPass : public renderer::RenderPass {
public:
    MousePickingPass(event::Dispatcher& dispatcher) : RenderPass("MousePicking Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/editor/mouse_picking.vert");
        shader.addShader("../../../assets/shaders/editor/mouse_picking.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~MousePickingPass() override {}

    void render(entt::registry& registry) override {
        assert(renderContext->contains("view"));
        assert(renderContext->contains("projection"));
        
        glEnable(GL_DEPTH_TEST);
        shader.mat4f("view", glm::value_ptr(renderContext->at("view").as<glm::mat4>()));
        shader.mat4f("projection", glm::value_ptr(renderContext->at("projection").as<glm::mat4>()));
        for (auto [ent, mesh] : registry.view<std::shared_ptr<renderer::Mesh>>().each()) {
            shader.veci("entID", static_cast<int>(ent));
            mesh->draw(shader, {}, false);
        }
    }

    void UI() override {

    }

private:

};

#endif