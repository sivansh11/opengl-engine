#include <iostream>

#include "window.hpp"
#include "gfx/texture.hpp"
#include "mesh.hpp"
#include "gfx/shaders.hpp"

int main() {
    core::Window window("hello", 640, 420);

    std::vector<core::Vertex> verts{
        core::Vertex{{-1,  1, 0}, {}, {0, 1}},
        core::Vertex{{-1, -1, 0}, {}, {0, 0}},
        core::Vertex{{ 1,  1, 0}, {}, {1, 1}},
        core::Vertex{{ 1, -1, 0}, {}, {1, 0}},
    };

    std::vector<uint32_t> indices{
        0, 1, 2,
        1, 2, 3,
    };

    core::Mesh mesh(verts, indices);

    core::gfx::ShaderProgram shader{};
    shader.addShader("../../../apps/texture_test/shaders/tex.vert");
    shader.addShader("../../../apps/texture_test/shaders/tex.frag");
    shader.link();

    core::gfx::Texture2D texture("../../../apps/texture_test/textures/hamster.png");

    while (!window.shouldClose()) {
        window.pollEvents();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
        texture.bind("tex", 0, shader);
        mesh.draw();

        window.updateScreen();
    }
    

    return 0;
}