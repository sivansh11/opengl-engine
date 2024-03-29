#ifndef RENDERER_GEOMETRY_PASS_HPP
#define RENDERER_GEOMETRY_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class GeometryPass : public RenderPass {
public:
    GeometryPass(event::Dispatcher& dispatcher) : RenderPass("Geometry Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/geometry/geometry.vert");
        shader.addShader("../../../assets/shaders/geometry/geometry.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~GeometryPass() override {}

    void render(entt::registry& registry) override {
        assert(renderContext->contains("view"));
        assert(renderContext->contains("projection"));
        
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        shader.mat4f("view", glm::value_ptr(renderContext->at("view").as<glm::mat4>()));
        shader.mat4f("projection", glm::value_ptr(renderContext->at("projection").as<glm::mat4>()));
        for (auto [ent, cc] : registry.view<core::ChildrenComponent>().each()) {
            auto transformComponent = registry.get<core::TransformComponent>(ent);
            for (auto ent : cc.children) {
                auto mesh = registry.get<std::shared_ptr<Mesh>>(ent);
                mesh->draw(shader, transformComponent);
            }
        }
        glEnable(GL_CULL_FACE);
    }

    void UI() override {

    }

private:

};

} // namespace renderer

#endif