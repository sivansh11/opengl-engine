#ifndef RENDERER_GEOMETRY_HPP
#define RENDERER_GEOMETRY_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class GeometryPass : public RenderPass {
public:
    GeometryPass() : RenderPass("Geometry Pass") {
        shader.addShader("../../../assets/shaders/geometry/geometry.vert");
        shader.addShader("../../../assets/shaders/geometry/geometry.frag");
        shader.link();
    }

    ~GeometryPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        assert(renderContext.contains("view"));
        assert(renderContext.contains("projection"));

        glEnable(GL_DEPTH_TEST);
        shader.mat4f("view", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["view"])));
        shader.mat4f("projection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["projection"])));
        auto view = registry.view<Model, core::TransformComponent>();
        for (auto ent : view) {
            auto [model, transform] = registry.get<Model, core::TransformComponent>(ent);
            model.draw(shader, transform);
        }
    }

private:

};

} // namespace renderer

#endif