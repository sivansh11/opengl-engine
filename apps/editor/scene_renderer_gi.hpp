#ifndef SCENE_RENDERER_GI_HPP
#define SCENE_RENDERER_GI_HPP

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

class SceneRendererGI : public core::Panel {
public:
    SceneRendererGI(core::Window& window) : window(window), Panel("Renderer") {
        
        voxels = gfx::Texture::Builder()
                        .setMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_NEAREST)
                        .setMagFilter(gfx::Texture::MagFilter::eLINEAR)
                        .build(gfx::Texture::Type::e3D);

        gfx::Texture::CreateInfo createInfo{};
        createInfo.width = voxelTextureSize;
        createInfo.height = voxelTextureSize;
        createInfo.depth = voxelTextureSize;
        createInfo.genMipMap = true;
        createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA8;
        voxels->create(createInfo);

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

        shader.addShader("../../../assets/shaders/directional_light.vert");
        shader.addShader("../../../assets/shaders/directional_light.frag");
        shader.link();
        
        depthShader.addShader("../../../assets/shaders/directional_depthmap.vert");
        depthShader.addShader("../../../assets/shaders/directional_depthmap.frag");
        depthShader.link();

        // voxelizationPass.addShader("../../../assets/shaders/voxels/voxelize.vert");
        // voxelizationPass.addShader("../../../assets/shaders/voxels/voxelize.frag");
        // voxelizationPass.addShader("../../../assets/shaders/voxels/voxelize.geom");
        // voxelizationPass.link();

        // voxelTracePass.addShader("../../../assets/shaders/voxels/voxel_cone_tracing.vert");
        // voxelTracePass.addShader("../../../assets/shaders/voxels/voxel_cone_tracing.frag");
        // voxelTracePass.link();

        // voxelVisulization.addShader("../../../assets/shaders/voxels/voxelize.vert");
        // voxelVisulization.addShader("../../../assets/shaders/voxels/voxelize.frag");
        // voxelVisulization.addShader("../../../assets/shaders/voxels/voxelize.geom");
        // voxelVisulization.link();

        lightBuffer = gfx::Buffer(1, gfx::Buffer::Useage::eDYNAMIC_DRAW);

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

        // if (true) {
        //     glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "voxelizationPass\0");
        //     voxelizationPass.bind();
        //     glViewport(0, 0, voxelTextureSize, voxelTextureSize);
        //     glDisable(GL_CULL_FACE);
        //     glDisable(GL_DEPTH_TEST);
        //     glDisable(GL_BLEND);

        //     voxels->bind("voxels", 0, voxelizationPass);
        //     lightBuffer.bind(0);

        //     for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
        //         model.draw(voxelizationPass, tc);
        //     }

        //     glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        //     glPopDebugGroup();

        // }

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
        glEnable(GL_DEPTH_TEST);
        switch (current)
        {
        case 0:
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "normal\0");
            shader.bind();
            depthMap.getTexture(gfx::FrameBuffer::Attachment::eDEPTH)->bind("depthMap", 4, shader);
            shader.mat4f("view", glm::value_ptr(camera.getView()));
            shader.mat4f("projection", glm::value_ptr(camera.getProjection()));
            shader.vec3f("viewPos", glm::value_ptr(camera.getPos()));
            shader.veci("numLights", lights.size());
            shader.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
            shader.vec3f("directionalLight.pos", glm::value_ptr(directionalLight.pos));
            shader.vec3f("directionalLight.color", glm::value_ptr(directionalLight.color));
            shader.vec3f("directionalLight.ambience", glm::value_ptr(directionalLight.ambience));
            lightBuffer.bind(0);
            for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
                model.draw(shader, tc);
            }
            break;
        case 1:
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "voxelRT\0");
            voxelTracePass.bind();
            voxels->bind("voxels", 10, voxelTracePass);
            voxelTracePass.mat4f("view", glm::value_ptr(camera.getView()));
            voxelTracePass.mat4f("projection", glm::value_ptr(camera.getProjection()));
            voxelTracePass.vec3f("viewPos", glm::value_ptr(camera.getPos()));
            voxelTracePass.veci("numLights", lights.size());
            voxelTracePass.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
            voxelTracePass.vec3f("directionalLight.pos", glm::value_ptr(directionalLight.pos));
            voxelTracePass.vec3f("directionalLight.color", glm::value_ptr(directionalLight.color));
            voxelTracePass.vec3f("directionalLight.ambience", glm::value_ptr(directionalLight.ambience));
            lightBuffer.bind(0);
            for (auto [ent, model, tc] : scene->group<renderer::Model, core::TransformComponent>()) {
                model.draw(voxelTracePass, tc);
            }
            break;
        
        case 2:
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "visulize\0");

            voxelVisulization.veci("Dimensions", voxelTextureSize);
            voxelVisulization.veci("TotalNumVoxels", voxelTextureSize * voxelTextureSize * voxelTextureSize);
            voxelVisulization.vecf("VoxelSize", 150.0f / voxelTextureSize);
            float voxelSize = 150.f / voxelTextureSize;

            glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(voxelSize)), glm::vec3(0, 0, 0));
            glm::mat4 viewMatrix = camera.getView();
            glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
            glm::mat4 projectionMatrix = camera.getProjection();
            
            voxelVisulization.mat4f("ModelViewMatrix", glm::value_ptr(modelViewMatrix));
            voxelVisulization.mat4f("ProjectionMatrix", glm::value_ptr(projectionMatrix));

            voxels->bind("voxels", 0, voxelVisulization);

            voxelVao.bind();
            glDrawArrays(GL_POINTS, 0, voxelTextureSize * voxelTextureSize * voxelTextureSize);

            break;

        // default:
        //     break;
        }
        
        glPopDebugGroup();
        viewPort.unbind();
        glPopDebugGroup();
    }

    void onImGuiRender() {
        if (!m_show) return;
        ImGui::Begin("Renderer Settings");
        if (ImGui::Button("Reload shaders")) {
            shader.reload();
            depthShader.reload();
        }
        ImGui::SliderInt("shaderSelector", &current, 0, 2);
        ImGui::End();
    }

    GLuint getRenderedImage() {
        return viewPort.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0)->getID();
    }

private:
    core::Window& window;
    
    bool voxelize = false;

    int current = 0;

    gfx::FrameBuffer viewPort;
    gfx::FrameBuffer depthMap;
    // gfx::FrameBuffer gBuffer;

    gfx::ShaderProgram shader;
    gfx::ShaderProgram voxelTracePass;
    gfx::ShaderProgram voxelizationPass;
    gfx::ShaderProgram voxelVisulization;
    gfx::ShaderProgram depthShader;

    gfx::VertexAttribute voxelVao;

    const int voxelTextureSize = 64;

    std::shared_ptr<gfx::Texture> voxels;

    gfx::Buffer lightBuffer;
    std::vector<core::PointLightComponent> lights;

    gfx::Buffer quadBuffer{};
    gfx::VertexAttribute quadBufferVA;
};

#endif