#ifndef RENDERER_VXGI_HPP
#define RENDERER_VXGI_HPP

#include "../renderpass.hpp"

#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class VXGIPass : public RenderPass {
public:
    VXGIPass(event::Dispatcher& dispatcher) : RenderPass("VXGI Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/vxgi.vert");
        shader.addShader("../../../assets/shaders/voxels/vxgi.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });

        // texNoise->loadImage("../../../assets/texture/noise.jpg");
        pointLigthBuffer = gfx::Buffer{gfx::Buffer::Useage::eDYNAMIC_DRAW};

        vertexBuffer = gfx::Buffer{static_cast<uint32_t>(gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0])), gfx::Buffer::Useage::eDYNAMIC_DRAW, "PointLightBuffer"};
        vertexBuffer.push(gfx::frameBufferQuadVertices.data());
        vao.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        vao.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
        vao.bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);
    }

    ~VXGIPass() override {

    } 

    void render(entt::registry& registry) override {
        renderContext->at("maxDist") = maxDist;
        renderContext->at("maxCount") = maxCount;
        renderContext->at("tanHalfAngles") = tanHalfAngles;
        renderContext->at("alphaThresh") = alphaThresh;
        
        if (renderContext->at("showing").as<std::string>() != "vxgiFinalImage") return;
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            break;  
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());

        shader.mat4f("view", glm::value_ptr(renderContext->at("view").as<glm::mat4>()));
        shader.mat4f("projection", glm::value_ptr(renderContext->at("projection").as<glm::mat4>()));
        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));
        
        shader.vec3f("viewPos", glm::value_ptr(renderContext->at("viewPos").as<glm::vec3>()));

        shader.veci("hasDirectionalLight", true);
        renderContext->at("depthMap").as<std::shared_ptr<gfx::Texture>>()->bind("depthMap", 4, shader);
        shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));
        
        renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>()->bind("voxels", 7, shader);
        shader.veci("voxelDim", renderContext->at("voxelDim").as<int>());
        shader.vecf("voxelGridSize", renderContext->at("voxelGridSize").as<float>());
        shader.veci("samples", samples);
        shader.vecf("MAX_DIST", maxDist);
        shader.vecf("ALPHA_THRESH", alphaThresh);
        shader.veci("MAX_COUNT", maxCount);
        shader.vecf("tanHalfAngle", tanHalfAngles);

        renderContext->at("texAlbedoSpec").as<std::shared_ptr<gfx::Texture>>()->bind("texAlbedoSpec", 0, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 1, shader);
        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 2, shader);

        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        if (pointLights.size() > 0) {
            pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
            pointLigthBuffer.push(pointLights.data());
        } else {

        }
        shader.veci("numLights", pointLights.size());
        shader.veci("outputType", outColor);
        pointLigthBuffer.bind(0);

        // vao.bind();
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        auto view = registry.view<Model, core::TransformComponent>();
        for (auto ent : view) {
            auto [model, transform] = registry.get<Model, core::TransformComponent>(ent);
            model.draw(shader, transform);
        }
    }

    void UI() override {
        ImGui::DragFloat("AlphaThresh", &alphaThresh, .001, .01, .99);
        ImGui::DragFloat("MaxDist", &maxDist, 1, 0, 5000);
        ImGui::DragInt("samples", &samples, 1, 1, 15);
        ImGui::DragInt("maxCount", &maxCount, 1, 1, 30);
        ImGui::DragFloat("tanHalfAngles", &tanHalfAngles, 0.01, 0, 100);
        ImGui::DragInt("outColor", &outColor, 1, 0, 5);

    }

private:
    float alphaThresh = .99;
    float maxDist = 1000;
    int samples = 1;
    int maxCount = 25;
    float tanHalfAngles = 1;
    // std::shared_ptr<gfx::Texture> texNoise;
    gfx::Buffer pointLigthBuffer;
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
    int outColor = 0;

};

} // namespace renderer

#endif