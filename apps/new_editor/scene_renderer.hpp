#ifndef SCENE_REDNERER_HPP
#define SCENE_REDNERER_HPP

#include "event_types.hpp"
#include "ecs.hpp"

#include "gfx/framebuffer.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"
#include "gfx/shaders.hpp"

#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"

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

        dispatcher.subscribe<core::ViewPortSizeUpdateEvent>([this](const event::Event& e) {  // callback for viewport size update event
            const core::ViewPortSizeUpdateEvent& event = reinterpret_cast<const core::ViewPortSizeUpdateEvent&>(e);
            this->gBuffer.invalidate(event.width, event.height);
        });

        core::gfx::FrameBufferInfo viewPortInfo{};           // viewport
        viewPortInfo.width = 100, viewPortInfo.height = 100; // temporaries
        viewPortInfo.attachments.push_back({GL_COLOR_ATTACHMENT0, {GL_RGBA16F, GL_FLOAT}});
        // viewPortInfo.attachments.push_back({GL_DEPTH_ATTACHMENT, {GL_DONT_CARE, GL_DONT_CARE}});
        viewPort = core::gfx::FrameBuffer{viewPortInfo};

        dispatcher.subscribe<core::ViewPortSizeUpdateEvent>([this](const event::Event& e) {  // callback for viewport size update event
            const core::ViewPortSizeUpdateEvent& event = reinterpret_cast<const core::ViewPortSizeUpdateEvent&>(e);
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
    }

    ~SceneRenderer() {

    }

    void render(ecs::Scene& scene, ecs::EntityID cameraEnt) {
        if (cameraEnt == ecs::null) {
            viewPort.bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            viewPort.unbind();
            return;
        }
        auto& camera = scene.get<core::Camera>(cameraEnt);

        lights.clear();
        for (auto [ent, pl] : ecs::SceneView<core::PointLight>(scene)) {
            core::LightData& lightData = lights.emplace_back();
            lightData.pos = pl.pos;
            lightData.color = pl.color;
            lightData.term = pl.term;
            if (scene.has<core::AmbienceLight>(ent)) 
                lightData.ambience = scene.get<core::AmbienceLight>(ent).ambient;
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
        gBuffer.bind();
        glClearColor(0, 0, 0, 1);   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        geometryPass.bind();
        geometryPass.mat4f("mvp.view", glm::value_ptr(camera.getView()));
        geometryPass.mat4f("mvp.projection", glm::value_ptr(camera.getProjection()));
        for (auto [ent, model, transform] : ecs::SceneView<core::Model, core::Transform>(scene)) {
            geometryPass.mat4f("mvp.model", glm::value_ptr(transform.mat4()));
            model.draw(geometryPass);
        }
        gBuffer.unbind();

        glDisable(GL_DEPTH_TEST);
        viewPort.bind();
        glClearColor(0, 0, 0, 1);   
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingPass.bind();
        lightingPass.veci("numLights", lights.size());
        lightingPass.vec3f("viewPos", glm::value_ptr(camera.getPosition())); 
        glBindTextureUnit(0, gBuffer.getTextureID(GL_COLOR_ATTACHMENT0));
        glBindTextureUnit(1, gBuffer.getTextureID(GL_COLOR_ATTACHMENT1));
        glBindTextureUnit(2, gBuffer.getTextureID(GL_COLOR_ATTACHMENT2));
        lightBuffer.bind(0);
        frameBufferVertexAttribute.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        viewPort.unbind();
    }

    GLuint displayViewPort() {
        return viewPort.getTextureID(GL_COLOR_ATTACHMENT0);
    }

private:
    core::gfx::FrameBuffer gBuffer;
    core::gfx::FrameBuffer viewPort;

    core::gfx::Buffer frameBufferQuadBuffer;
    core::gfx::VertexAttribute frameBufferVertexAttribute;

    core::gfx::ShaderProgram geometryPass;
    core::gfx::ShaderProgram lightingPass;

    core::gfx::Buffer lightBuffer;
    std::vector<core::LightData> lights;

    core::LightData *mappedMemory;
};

#endif