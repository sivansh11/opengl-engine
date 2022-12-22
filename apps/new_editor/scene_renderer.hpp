#ifndef SCENE_REDNERER_HPP
#define SCENE_REDNERER_HPP

#include "event_types.hpp"
#include "entity.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"
#include "gfx/shaders.hpp"

#include "model.hpp"
#include "components/camera.hpp"
#include "components/light.hpp"

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstring>

class SceneRenderer {
public:
    SceneRenderer(event::Dispatcher& dispatcher) {

        core::gfx::FrameBufferInfo gBufferInfo{};          // intermediate gbuffer
        gBufferInfo.width = 100, gBufferInfo.height = 100; // temporaries
        gBufferInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA16F, GL_FLOAT}});
        gBufferInfo.attachments.push_back({GL_COLOR_ATTACHMENT1, {GL_RGBA16F, GL_FLOAT}});
        gBufferInfo.attachments.push_back({GL_COLOR_ATTACHMENT2, {GL_RGBA8, GL_UNSIGNED_BYTE}});
        gBufferInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DONT_CARE, GL_DONT_CARE}});
        gBuffer = core::gfx::FrameBuffer{gBufferInfo};

        dispatcher.subscribe<core::ViewPortResizeEvent>([this](const event::Event& e) {  // callback for viewport size update event
            const core::ViewPortResizeEvent& event = reinterpret_cast<const core::ViewPortResizeEvent&>(e);
            this->gBuffer.invalidate(event.width, event.height);
        });

        core::gfx::FrameBufferInfo viewPortInfo{};           // viewport
        viewPortInfo.width = 100, viewPortInfo.height = 100; // temporaries
        viewPortInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA16F, GL_FLOAT}});
        viewPortInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DONT_CARE, GL_DONT_CARE}});
        viewPort = core::gfx::FrameBuffer{viewPortInfo};

        dispatcher.subscribe<core::ViewPortResizeEvent>([this](const event::Event& e) {  // callback for viewport size update event
            const core::ViewPortResizeEvent& event = reinterpret_cast<const core::ViewPortResizeEvent&>(e);
            this->viewPort.invalidate(event.width, event.height);
        });

        frameBufferQuadBuffer = core::gfx::Buffer(sizeof(core::gfx::FrameBufferVertex) * core::gfx::frameBufferQuadVertices.size());
        frameBufferQuadBuffer.push(core::gfx::frameBufferQuadVertices.data());
        frameBufferVertexAttribute = core::gfx::VertexAttribute{};
        frameBufferVertexAttribute.attributeLocation(0, 2, offsetof(core::gfx::FrameBufferVertex, pos));
        frameBufferVertexAttribute.attributeLocation(1, 2, offsetof(core::gfx::FrameBufferVertex, uv));
        frameBufferVertexAttribute.bindVertexBuffer<core::gfx::FrameBufferVertex>(frameBufferQuadBuffer);

        geometryPass.addShader("../../../apps/new_editor/shaders/gbuffer.vert");
        geometryPass.addShader("../../../apps/new_editor/shaders/gbuffer.frag");
        geometryPass.link();

        lightingPass.addShader("../../../apps/new_editor/shaders/light_pass.vert");
        lightingPass.addShader("../../../apps/new_editor/shaders/light_pass.frag");
        lightingPass.link();          
        lightingPass.bind();
        lightingPass.veci("gBuffer.posBuffer", 0);
        lightingPass.veci("gBuffer.norBuffer", 1);
        lightingPass.veci("gBuffer.albedoSpecBuffer", 2); 

        lightBuffer = core::gfx::Buffer{1, core::gfx::BufferType::DYNAMIC_BUFFER};

        mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(core::gfx::BufferOperation::WRITE_OPERATION));

        core::gfx::FrameBufferInfo depthMapInfo{};
        depthMapInfo.width = 1024;
        depthMapInfo.height = 1024;
        depthMapInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DEPTH_COMPONENT16, GL_FLOAT}});
        depthMap = core::gfx::FrameBuffer{depthMapInfo};
        glNamedFramebufferDrawBuffer(depthMap.getID(), GL_NONE);
        glNamedFramebufferReadBuffer(depthMap.getID(), GL_NONE);

        directionalLightPass.addShader("../../../apps/new_editor/shaders/directional_light_pass.vert");
        directionalLightPass.addShader("../../../apps/new_editor/shaders/directional_light_pass.frag");
        directionalLightPass.link();
    }

    ~SceneRenderer() {

    }

    void render(core::Scene32& scene, core::EditorCamera& camera) {
        // if (cameraEnt == ecs::null) {
        //     viewPort.bind();
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //     viewPort.unbind();
        //     return;
        // }

        lights.clear();
        for (auto [ent, pl, t] : scene.group<core::PointLightComponent, core::TransformComponent>()) {
            core::LightData& lightData = lights.emplace_back();
            lightData.pos = t.translation;
            lightData.color = pl.color;
            lightData.term = pl.term;
            if (scene.has<core::AmbienceLightComponent>(ent)) 
                lightData.ambience = scene.get<core::AmbienceLightComponent>(ent).ambient;
        }
        uint32_t bufferSize = sizeof(core::LightData) * lights.size();
        if (lightBuffer.capacity() < bufferSize) {
            lightBuffer.unmap();
            lightBuffer.resize(bufferSize);
            mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(core::gfx::BufferOperation::WRITE_OPERATION));
        }
        if (lights.size() > 0)
            std::memcpy(mappedMemory, lights.data(), bufferSize);

        glEnable(GL_DEPTH_TEST);
        depthMap.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float nearPLane = 0.01f;
        float farPlane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPLane, farPlane);
        glm::mat4 lightView = glm::lookAt(glm::vec3{-200.0f, 400.0f, -100.0f},
                                          glm::vec3{0.0, 0.0, 0.0},
                                          glm::vec3{0, 1, 0});
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        directionalLightPass.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        for (auto [ent, model, transform] : scene.group<core::Model, core::TransformComponent>()) {
            directionalLightPass.mat4f("model", glm::value_ptr(transform.mat4()));
            model.draw(directionalLightPass, false);
        }
        depthMap.unbind();

        glEnable(GL_DEPTH_TEST);
        gBuffer.bind();
        glClearColor(0, 0, 0, 1);   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        geometryPass.bind();
        geometryPass.mat4f("mvp.view", glm::value_ptr(camera.getView()));
        geometryPass.mat4f("mvp.projection", glm::value_ptr(camera.getProjection()));
 
        for (auto [ent, model, transform] : scene.group<core::Model, core::TransformComponent>()) {
            geometryPass.mat4f("mvp.model", glm::value_ptr(transform.mat4()));
            model.draw(geometryPass);
        }
        gBuffer.unbind();

        // lightingPass.veci("shadowMap", depthMap.getTextureID(GL_DEPTH_ATTACHMENT));
        glDisable(GL_DEPTH_TEST);
        viewPort.bind();
        glClearColor(0, 0, 0, 1);   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingPass.bind();
        lightingPass.veci("numLights", lights.size());
        lightingPass.vec3f("viewPos", glm::value_ptr(camera.getPos())); 
        glBindTextureUnit(0, gBuffer.getTextureID(GL_COLOR_ATTACHMENT0));
        glBindTextureUnit(1, gBuffer.getTextureID(GL_COLOR_ATTACHMENT1));
        glBindTextureUnit(2, gBuffer.getTextureID(GL_COLOR_ATTACHMENT2));
        lightBuffer.bind(0);
        frameBufferVertexAttribute.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        viewPort.unbind();
    }

    GLuint shadow() {
        return depthMap.getTextureID(GL_DEPTH_ATTACHMENT);
    }

    GLuint displayViewPort() {
        return viewPort.getTextureID(GL_COLOR_ATTACHMENT0);
    }

private:
    core::gfx::FrameBuffer depthMap;
    core::gfx::FrameBuffer gBuffer;
    core::gfx::FrameBuffer viewPort;

    core::gfx::Buffer frameBufferQuadBuffer;
    core::gfx::VertexAttribute frameBufferVertexAttribute;

    core::gfx::ShaderProgram geometryPass;
    core::gfx::ShaderProgram lightingPass;
    core::gfx::ShaderProgram directionalLightPass;

    core::gfx::Buffer lightBuffer;
    std::vector<core::LightData> lights;

    core::LightData *mappedMemory;
};

// class SceneRenderer {
// public:
//     SceneRenderer(event::Dispatcher& dispatcher) {

//         core::gfx::FrameBufferInfo viewPortInfo{};           // viewport
//         viewPortInfo.width = 100, viewPortInfo.height = 100; // temporaries
//         viewPortInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA16F, GL_FLOAT}});
//         viewPortInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DONT_CARE, GL_DONT_CARE}});
//         viewPort = core::gfx::FrameBuffer{viewPortInfo};

//         dispatcher.subscribe<core::ViewPortResizeEvent>([this](const event::Event& e) {  // callback for viewport size update event
//             const core::ViewPortResizeEvent& event = reinterpret_cast<const core::ViewPortResizeEvent&>(e);
//             this->viewPort.invalidate(event.width, event.height);
//         });


        
//         shader.addShader("../../../apps/new_editor/shaders/gbuffer.vert");
//         shader.addShader("../../../apps/new_editor/shaders/blinn-phong.frag");
//         shader.link();

//         lightBuffer = core::gfx::Buffer{1, core::gfx::BufferType::DYNAMIC_BUFFER};

//         mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(core::gfx::BufferOperation::WRITE_OPERATION));
//     }

//     ~SceneRenderer() {

//     }

//     void render(core::Scene32& scene, core::EditorCamera& camera) {
//         // if (cameraEnt == ecs::null) {
//         //     viewPort.bind();
//         //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//         //     viewPort.unbind();
//         //     return;
//         // }

//         lights.clear();
//         for (auto [ent, pl] : scene.group<core::PointLightComponent>()) {
//             core::LightData& lightData = lights.emplace_back();
//             lightData.pos = pl.pos;
//             lightData.color = pl.color;
//             lightData.term = pl.term;
//             if (scene.has<core::AmbienceLightComponent>(ent)) 
//                 lightData.ambience = scene.get<core::AmbienceLightComponent>(ent).ambient;
//         }
//         uint32_t bufferSize = sizeof(core::LightData) * lights.size();
//         if (lightBuffer.capacity() < bufferSize) {
//             lightBuffer.unmap();
//             lightBuffer.resize(bufferSize);
//             mappedMemory = reinterpret_cast<core::LightData *>(lightBuffer.map(core::gfx::BufferOperation::WRITE_OPERATION));
//         }
//         if (lights.size() > 0)
//             std::memcpy(mappedMemory, lights.data(), bufferSize);

//         glEnable(GL_DEPTH_TEST);
//         viewPort.bind();
//         glClearColor(1, 0, 1, 1);   
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//         shader.bind();
//         shader.mat4f("mvp.view", glm::value_ptr(camera.getView()));
//         shader.mat4f("mvp.projection", glm::value_ptr(camera.getProjection()));
//         shader.veci("numLights", lights.size());
//         shader.vec3f("viewPos", glm::value_ptr(camera.getPos())); 
//         lightBuffer.bind(0);
//         for (auto [ent, model, transform] : scene.group<core::Model, core::TransformComponent>()) {
//             shader.mat4f("mvp.model", glm::value_ptr(transform.mat4()));
//             model.draw(shader);
//         }
//         viewPort.unbind();
//     }

//     GLuint displayViewPort() {
//         return viewPort.getTextureID(GL_COLOR_ATTACHMENT0);
//     }

// private:
//     core::gfx::FrameBuffer viewPort;

//     core::gfx::ShaderProgram shader;

//     core::gfx::Buffer lightBuffer;
//     std::vector<core::LightData> lights;

//     core::LightData *mappedMemory;
// };


#endif