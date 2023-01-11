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
        renderer::Vertex{{-0.5f, -0.5f, 0}, {}, {0, 0}},
        renderer::Vertex{{-0.5f,  0.5f, 0}, {}, {0, 1}},
        renderer::Vertex{{ 0.5f,  0.5f, 0}, {}, {1, 1}},
        renderer::Vertex{{-0.5f, -0.5f, 0}, {}, {0, 0}},
        renderer::Vertex{{ 0.5f,  0.5f, 0}, {}, {1, 1}},
        renderer::Vertex{{ 0.5f, -0.5f, 0}, {}, {1, 0}},
    };
    std::vector<uint32_t> indices {
        0, 1, 2, 3, 4, 5
    };

    gfx::ShaderProgram shader{};
    shader.addShader(ASSETS_PATH + "/shaders/test.vert");
    shader.addShader(ASSETS_PATH + "/shaders/test_.frag");
    shader.link();

    renderer::Mesh mesh{vertices, indices};

    auto texture = gfx::Texture::Builder().build(gfx::Texture::Type::e2D);
    texture->loadImage(ASSETS_PATH + "/icons/DirectoryIcon.png");

    gfx::Texture::Builder builder;
    
    gfx::FrameBuffer fb = gfx::FrameBuffer::Builder(100, 100)
                .addAttachment(builder, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
                .addAttachment(builder, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32F, gfx::FrameBuffer::Attachment::eDEPTH)
                .build();

    ImVec2 size {100, 100};
    while (!window.shouldClose()) {
        window.pollEvents();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fb.bind(size.x, size.y);
        fb.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
        shader.bind();
        texture->bind("tex", 0, shader);
        mesh.draw(shader, {});
        fb.unbind();

        core::startFrameImgui();
        ImGui::Begin("T");
        ImGui::Image((void *)(fb.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0)->getID()), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        size = ImGui::GetWindowSize();
        fb.invalidate(size.x, size.y);
        ImGui::End();
        core::endFrameImgui(window);

        window.updateScreen();
    }
    
}