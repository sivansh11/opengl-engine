#ifndef SCENE_RENDERER_HPP
#define SCENE_RENDERER_HPP

#include "core/entity.hpp"
#include "core/window.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"

#include "renderer/model.hpp"

#include "custom_events.hpp"
#include "editor_camera.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <memory>

class SceneRenderer {
public:
    SceneRenderer(core::Window& window) : window(window) {
        gfx::FrameBufferInfo viewPortInfo{};
        viewPortInfo.width = 100, viewPortInfo.height = 100; // temp dims
        // viewPortInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA16F, GL_FLOAT}});
        viewPortInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA8, GL_UNSIGNED_BYTE}});
        viewPortInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DEPTH_COMPONENT, GL_FLOAT}});
        viewPort = gfx::FrameBuffer(viewPortInfo);

        window.getDispatcher().subscribe<ViewPortResizeEvent>([this](const event::Event& e) {
            const ViewPortResizeEvent& event = reinterpret_cast<const ViewPortResizeEvent&>(e);
            this->viewPort.invalidate(event.width, event.height);
        });

        shader.addShader("../../../assets/shaders/default.vert");
        shader.addShader("../../../assets/shaders/default.frag");
        shader.link();

        lightBuffer = gfx::Buffer(1, gfx::BufferType::DYNAMIC_BUFFER);
        mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(gfx::BufferOperation::WRITE_OPERATION));   
    }

    ~SceneRenderer() {

    }

    void render(std::shared_ptr<core::Scene> scene, EditorCamera camera) {
        lights.clear();
        for (auto [ent, ld] : scene->group<core::LightData>()) {
            lights.emplace_back(ld);
        }
        uint32_t bufferSize = sizeof(core::LightData) * lights.size();
        if (lightBuffer.capacity() < bufferSize) {
            lightBuffer.unmap();
            lightBuffer.resize(bufferSize);
            mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(gfx::BufferOperation::WRITE_OPERATION));   
        }
        if (lights.size() > 0) {
            std::memcpy(mappedMemory, lights.data(), bufferSize);
        }

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "viewport\0");
        viewPort.bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.bind();
        shader.mat4f("view", glm::value_ptr(camera.getView()));
        shader.mat4f("projection", glm::value_ptr(camera.getProjection()));
        shader.veci("numLights", lights.size());
        shader.vec3f("viewPos", glm::value_ptr(camera.getPos()));
        lightBuffer.bind(0);
        for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
            model.draw(shader, tc);
        }
        viewPort.unbind();
        glPopDebugGroup();
    }

    GLuint getRender() {
        return viewPort.getTextureID(GL_COLOR_ATTACHMENT0).getID(   );
    }

    void imguiRender() {}

private:
    core::Window& window;

    gfx::FrameBuffer viewPort;

    gfx::ShaderProgram shader;

    gfx::Buffer lightBuffer;
    std::vector<core::LightData> lights;

    core::LightData *mappedMemory;
};

#endif