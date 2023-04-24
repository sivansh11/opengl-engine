#ifndef RENDERER_VXGI_COMPOSITE_HPP
#define RENDERER_VXGI_COMPOSITE_HPP

#include "../renderpass.hpp"

#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class VXGICompositePass : public RenderPass {
public:
    VXGICompositePass(event::Dispatcher& dispatcher) : RenderPass("VXGIComposite Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/deferred_vxgi.vert");
        shader.addShader("../../../assets/shaders/voxels/deferred_vxgi.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });

        // todo find a common place for this
        pointLigthBuffer = gfx::Buffer{gfx::Buffer::Useage::eDYNAMIC_DRAW};

        // can be reused
        vertexBuffer = gfx::Buffer{static_cast<uint32_t>(gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0])), gfx::Buffer::Useage::eDYNAMIC_DRAW, "PointLightBuffer"};
        vertexBuffer.push(gfx::frameBufferQuadVertices.data());
        vao.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        vao.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
        vao.bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);
    }

    ~VXGICompositePass() override {

    } 

    void render(entt::registry& registry) override {
        // common to multiple passes
        // better to remove
        renderContext->at("maxDist") = maxDist;
        renderContext->at("maxCount") = maxCount;
        renderContext->at("tanHalfAngles") = tanHalfAngles;
        renderContext->at("alphaThresh") = alphaThresh;
        
        // effective final image
        // useful to turn off pass when not in use
        if (renderContext->at("showing").as<std::string>() != "vxgiFinalImage") return;
        
        // turn off renderpass
        // always running

        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            break;  
        }

        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        if (pointLights.size() > 0) {
            pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
            pointLigthBuffer.push(pointLights.data());
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());

        // non settings data
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));
        shader.vec3f("viewPos", glm::value_ptr(renderContext->at("viewPos").as<glm::vec3>()));
        shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));
        shader.veci("hasDirectionalLight", true);
        shader.veci("numPointLights", pointLights.size());
        renderContext->at("texAlbedoSpec").as<std::shared_ptr<gfx::Texture>>()->bind("texAlbedoSpec", 0, shader);
        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 1, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 2, shader);
        renderContext->at("texTangent").as<std::shared_ptr<gfx::Texture>>()->bind("texTangent", 8, shader);
        renderContext->at("depthMap").as<std::shared_ptr<gfx::Texture>>()->bind("shadowMap", 4, shader);
        renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>()->bind("voxels", 7, shader);
        renderContext->at("ssaoImage").as<std::shared_ptr<gfx::Texture>>()->bind("texSSAO", 3, shader);
        
        pointLigthBuffer.bind(0);
        
        // renderpass and pipeline settings data
        shader.veci("samples", samples);
        shader.vecf("MAX_DIST", maxDist);
        shader.vecf("ALPHA_THRESH", alphaThresh);
        shader.vecf("giBoost", giBoost);
        shader.veci("MAX_COUNT", maxCount);
        shader.vecf("tanHalfAngle", tanHalfAngles);
        shader.vecf("tanHalfAngleForOcclusion", tanHalfAngleForOcclusion);
        shader.veci("outputType", outColor);
        shader.veci("cone", cone);
        shader.veci("useSSAO", useSSAO);
        shader.veci("voxelDimensions", renderContext->at("voxelDimensions").as<int>());
        shader.vecf("voxelGridSize", renderContext->at("voxelGridSize").as<float>());

        // draw
        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {
        ImGui::DragFloat("AlphaThresh", &alphaThresh, .001, .01, .99);
        ImGui::DragFloat("MaxDist", &maxDist, 1, 0, 5000);
        ImGui::DragFloat("tanHalfAngles", &tanHalfAngles, 0.01, 0, 100);
        ImGui::DragFloat("tanHalfAngleForOcclusion", &tanHalfAngleForOcclusion, 0.01, 0, 100);
        ImGui::DragFloat("giBoost", &giBoost, 0.01, 0, 100);
        ImGui::DragInt("samples", &samples, 1, 1, 15);
        ImGui::DragInt("maxCount", &maxCount, 1, 1, 30);
        ImGui::DragInt("outColor", &outColor, 1, 0, 5);
        ImGui::Checkbox("Cone", &cone);
        ImGui::Checkbox("useSSAO", &useSSAO);

    }

private:
    float alphaThresh = .99;
    float maxDist = 1000000000;
    int samples = 1;
    int maxCount = 10000;
    float tanHalfAngles = .32;
    float tanHalfAngleForOcclusion = 1;
    float giBoost = 1;
    gfx::Buffer pointLigthBuffer;
    gfx::VertexAttribute vao;
    gfx::Buffer vertexBuffer;
    int outColor = 0;
    bool cone = false;
    bool useSSAO = false;
};

} // namespace renderer

#endif