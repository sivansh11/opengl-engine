#ifndef RENDERER_TEST_HPP
#define RENDERER_TEST_HPP

#include "../renderpass.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/vertex_attribute.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class Test : public RenderPass {
public:
    Test() : RenderPass("Test") {
        shader.addShader("../../../assets/shaders/test/test.vert");
        shader.addShader("../../../assets/shaders/test/test.frag");
        shader.link();
    }

    ~Test() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        shader.bind();
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:
    gfx::VertexAttribute vao;
    gfx::ShaderProgram shader;
};

} // namespace renderer

#endif