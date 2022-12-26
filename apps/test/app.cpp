#include "app.hpp"

#include "gfx/shaders.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"
#include "renderer/model.hpp"

#include "core/entity.hpp"
#include "core/serializer.hpp"

#include "core/components.hpp"

// #include "scene_renderer.hpp"

#include "core/imgui_utils.hpp"
#include "renderer/mesh.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Test", 640, 420) {
    core::initImgui(window);
    core::setupImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {

    std::unique_ptr<core::Scene> scene = std::make_unique<core::Scene>();

    std::vector<renderer::Vertex> vertices {
        renderer::Vertex{{-0.5f, -0.5f, 0}, {}, {}},
        renderer::Vertex{{-0.5f,  0.5f, 0}, {}, {}},
        renderer::Vertex{{ 0.5f,  0.5f, 0}, {}, {}},
        renderer::Vertex{{-0.5f, -0.5f, 0}, {}, {}},
        renderer::Vertex{{ 0.5f,  0.5f, 0}, {}, {}},
        renderer::Vertex{{ 0.5f, -0.5f, 0}, {}, {}},
    };
    std::vector<uint32_t> indices {
        0, 1, 2, 3, 4, 5
    };

    renderer::Mesh mesh{vertices, indices};

    gfx::ShaderProgram shader{};
    shader.addShader(ASSETS_PATH + "/shaders/test.vert");
    shader.addShader(ASSETS_PATH + "/shaders/test.frag");
    shader.link();

    while (!window.shouldClose()) {
        window.pollEvents();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
        mesh.draw(shader, {});

        core::startFrameImgui();

        core::endFrameImgui(window);

        window.updateScreen();
    }
    
}