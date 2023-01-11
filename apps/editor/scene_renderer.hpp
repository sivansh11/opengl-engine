#ifndef SCENE_RENDERER_HPP
#define SCENE_RENDERER_HPP

#include "core/entity.hpp"
#include "core/window.hpp"
#include "core/panel.hpp"

#include "gfx/framebuffer.hpp"
#include "gfx/shaders.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"

#include "renderer/model.hpp"
#include "renderer/base_renderer.hpp"

#include "custom_events.hpp"
#include "editor_camera.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <memory>

std::vector<glm::vec3> skyboxVertices = {
        // positions          
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},

        {-1.0f, -1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        {-1.0f,  1.0f, -1.0f},
        {1.0f,  1.0f, -1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        {1.0f, -1.0f,  1.0f},
    };

class SceneRenderer : public core::Panel {
public:
    SceneRenderer(core::Window& window) : window(window), Panel("Renderer") {
        gfx::Texture::Builder builder;

        builder.setMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_LINEAR);
        
        viewPort = gfx::FrameBuffer::Builder(100, 100)
                    .addAttachment(builder, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
                    .addAttachment(builder, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32F, gfx::FrameBuffer::Attachment::eDEPTH)
                    .build();
        viewPort.setClearColor(0, 0, 0, 1);
        viewPort.setClearDepth(1);

        depthMap = gfx::FrameBuffer::Builder(1080 * 4, 1080 * 4)
                    .addAttachment(builder, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32F, gfx::FrameBuffer::Attachment::eDEPTH)
                    .build();

        geometryPass.addShader("../../../assets/shaders/geometry_pass.vert");
        geometryPass.addShader("../../../assets/shaders/geometry_pass.frag");
        geometryPass.link();

        shader.addShader("../../../assets/shaders/directional_light.vert");
        shader.addShader("../../../assets/shaders/directional_light.frag");
        shader.link();
        
        depthShader.addShader("../../../assets/shaders/directional_depthmap.vert");
        depthShader.addShader("../../../assets/shaders/directional_depthmap.frag");
        depthShader.link();

        lightBuffer = gfx::Buffer(1, gfx::Buffer::Useage::eDYNAMIC_DRAW);

        skyBoxBuffer = gfx::Buffer(sizeof(glm::vec3) * skyboxVertices.size());
        skyBoxBuffer.push(skyboxVertices.data());
    
        skyBoxVertexAttribute.bindVertexBuffer<glm::vec3>(skyBoxBuffer);
        skyBoxVertexAttribute.attributeLocation(0, 3, 0);
    
        skyboxShader.addShader("../../../assets/shaders/sky_box.vert");
        skyboxShader.addShader("../../../assets/shaders/sky_box.frag");
        skyboxShader.link();

        quadBuffer = gfx::Buffer(sizeof(gfx::FrameBufferVertex) * gfx::frameBufferQuadVertices.size());
        quadBuffer.push(gfx::frameBufferQuadVertices.data());
        quadBufferVA.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        quadBufferVA.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, pos));
        quadBufferVA.bindVertexBuffer<gfx::FrameBufferVertex>(quadBuffer);
    }   

    void render(std::shared_ptr<core::Scene> scene, core::CameraComponent& camera, glm::vec2 size) {
        lights.clear();
        for (auto [ent, ld] : scene->group<core::PointLightComponent>()) {
            lights.emplace_back(ld);
        }
        uint32_t bufferSize = sizeof(core::PointLightComponent) * lights.size();
        if (lightBuffer.capacity() < bufferSize) {
            lightBuffer.resize(bufferSize);
        }
        if (lights.size() > 0) {
            lightBuffer.push(lights.data(), 0, lights.size() * sizeof(core::PointLightComponent));
        }  

        viewPort.invalidate(size.x, size.y);

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "depthMap\0");
        depthMap.bind();
        glEnable(GL_DEPTH_TEST);
        glClearDepthf(1);
        glClear(GL_DEPTH_BUFFER_BIT);
        core::DirectionalLightComponent directionalLight{};
        for (auto [ent, dl] : scene->group<core::DirectionalLightComponent>()) {
            directionalLight = dl;
        }
        glm::mat4 lightProjection = glm::ortho(-directionalLight.orthoProj, directionalLight.orthoProj, -directionalLight.orthoProj, directionalLight.orthoProj, directionalLight.near, directionalLight.far);  
        glm::mat4 lightView = glm::lookAt(directionalLight.pos * directionalLight.multiplier, 
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
        viewPort.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "skybox\0");
        glDepthMask(GL_FALSE);
        skyboxShader.bind();
        skyboxShader.mat4f("view", glm::value_ptr(glm::mat4{glm::mat3{camera.getView()}}));
        skyboxShader.mat4f("projection", glm::value_ptr(camera.getProjection()));
        skyBoxVertexAttribute.bind();
        skyBox.bind("skybox", 0, skyboxShader);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glPopDebugGroup();
        glEnable(GL_DEPTH_TEST);
        shader.bind();
        shader.mat4f("view", glm::value_ptr(camera.getView()));
        shader.mat4f("projection", glm::value_ptr(camera.getProjection()));
        shader.vec3f("viewPos", glm::value_ptr(camera.getPos()));
        shader.veci("numLights", lights.size());
        shader.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        shader.vec3f("directionalLight.pos", glm::value_ptr(directionalLight.pos));
        shader.vec3f("directionalLight.color", glm::value_ptr(directionalLight.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(directionalLight.ambience));
        depthMap.getTexture(gfx::FrameBuffer::Attachment::eDEPTH)->bind("depthMap", 4, shader);
        lightBuffer.bind(0);
        for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
            model.draw(shader, tc);
        }
        viewPort.unbind();
        glPopDebugGroup();
    }

    void onImGuiRender() {
        if (!m_show) return;
        ImGui::Begin("Renderer Settings");
        if (ImGui::Button("Reload shaders")) {
            skyboxShader.reload();
            shader.reload();
            depthShader.reload();
        }
        ImGui::End();
    }

    GLuint getRenderedImage() {
        return viewPort.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0)->getID();
    }

private:
    core::Window& window;

    gfx::ShaderProgram skyboxShader{};
    gfx::Buffer skyBoxBuffer;
    gfx::VertexAttribute skyBoxVertexAttribute{};
    gfx::CubeMap skyBox{"../../../assets/skybox/skybox/"};

    gfx::FrameBuffer viewPort;
    gfx::FrameBuffer depthMap;
    // gfx::FrameBuffer gBuffer;

    gfx::ShaderProgram shader;
    gfx::ShaderProgram geometryPass;
    gfx::ShaderProgram depthShader;

    gfx::Buffer lightBuffer;
    std::vector<core::PointLightComponent> lights;

    gfx::Buffer quadBuffer{};
    gfx::VertexAttribute quadBufferVA;
};

#endif