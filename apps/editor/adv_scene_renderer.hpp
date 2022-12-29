#ifndef ADV_SCENE_RENDERER_HPP
#define ADV_SCENE_RENDERER_HPP

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

class AdvSceneRenderer {
public:
    AdvSceneRenderer(core::Window& window) : window(window) {
        gfx::FrameBufferInfo viewPortInfo{};
        viewPortInfo.width = 100, viewPortInfo.height = 100; // temp dims
        viewPortInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA8, GL_UNSIGNED_BYTE}});
        viewPortInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DEPTH_COMPONENT, GL_FLOAT}});
        viewPort = gfx::FrameBuffer(viewPortInfo);

        window.getDispatcher().subscribe<ViewPortResizeEvent>([this](const event::Event& e) {
            const ViewPortResizeEvent& event = reinterpret_cast<const ViewPortResizeEvent&>(e);
            this->viewPort.invalidate(event.width, event.height);
        });

        gfx::FrameBufferInfo depthMapInfo{};
        depthMapInfo.width = 1080, depthMapInfo.height = 1080;
        depthMapInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DEPTH_COMPONENT, GL_FLOAT}});
        depthMap = gfx::FrameBuffer(depthMapInfo);
        glNamedFramebufferDrawBuffer(depthMap.getID(), GL_NONE);

        shader.addShader("../../../assets/shaders/directional_light.vert");
        shader.addShader("../../../assets/shaders/directional_light.frag");
        shader.link();
        
        depthShader.addShader("../../../assets/shaders/directional_depthmap.vert");
        depthShader.addShader("../../../assets/shaders/directional_depthmap.frag");
        depthShader.link();

        lightBuffer = gfx::Buffer(1, gfx::BufferType::DYNAMIC_BUFFER);
        mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(gfx::BufferOperation::WRITE_OPERATION));   
    }

    void render(std::shared_ptr<core::Scene> scene, EditorCamera camera) {
        // lights.clear();
        // for (auto [ent, ld] : scene->group<core::LightData>()) {
        //     lights.emplace_back(ld);
        // }
        // uint32_t bufferSize = sizeof(core::LightData) * lights.size();
        // if (lightBuffer.capacity() < bufferSize) {
        //     lightBuffer.unmap();
        //     lightBuffer.resize(bufferSize);
        //     mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(gfx::BufferOperation::WRITE_OPERATION));   
        // }
        // if (lights.size() > 0) {
        //     std::memcpy(mappedMemory, lights.data(), bufferSize);
        // }

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "depthMap\0");
        depthMap.bind();
        glEnable(GL_DEPTH_TEST);
        glClearDepthf(1);
        glClear(GL_DEPTH_BUFFER_BIT);
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);  
        glm::mat4 lightView = glm::lookAt(lightData.pos, 
                                  glm::vec3( 0.0f, 0.0f,  0.0f), 
                                  glm::vec3( 0.0f, 1.0f,  0.0f));  
        glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
        depthShader.bind();
        depthShader.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
            model.draw(depthShader, tc, false);
        }
        depthMap.unbind();
        glPopDebugGroup();

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "viewPort\0");
        viewPort.bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.bind();
        shader.mat4f("view", glm::value_ptr(camera.getView()));
        shader.mat4f("projection", glm::value_ptr(camera.getProjection()));
        shader.vec3f("viewPos", glm::value_ptr(camera.getPos()));
        shader.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        shader.vec3f("lightData.pos", glm::value_ptr(lightData.pos));
        shader.vec3f("lightData.ambience", glm::value_ptr(lightData.ambience));
        shader.vec3f("lightData.color", glm::value_ptr(lightData.color));
        shader.vec3f("lightData.term", glm::value_ptr(lightData.term));
        depthMap.getTextureID(GL_DEPTH_ATTACHMENT).bind("depthMap", 4, shader);
        lightBuffer.bind(0);
        for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
            model.draw(shader, tc);
        }
        viewPort.unbind();
        glPopDebugGroup();
    }

    void imguiRender() {
        ImGui::DragFloat3("lightPos", reinterpret_cast<float *>(&lightData.pos), 0.1);
        ImGui::DragFloat3("light term", reinterpret_cast<float *>(&lightData.term), 0.1);
    
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void*>(getDepthMap())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    }

    GLuint getRender() {
        return viewPort.getTextureID(GL_COLOR_ATTACHMENT0).getID();
    }

    GLuint getDepthMap() {
        return depthMap.getTextureID(GL_DEPTH_ATTACHMENT).getID();
    }

private:
    core::Window& window;

    core::LightData lightData{{0, 1, .6}, {1, 1, 1}, {.1, .1, .1}, {.01, .01, .01}};

    gfx::FrameBuffer viewPort;
    gfx::FrameBuffer depthMap;

    gfx::ShaderProgram shader;
    gfx::ShaderProgram depthShader;

    gfx::Buffer lightBuffer;
    std::vector<core::LightData> lights;

    core::LightData *mappedMemory;
};

#endif