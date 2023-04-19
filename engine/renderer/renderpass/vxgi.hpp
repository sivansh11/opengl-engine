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
    }

    ~VXGIPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            break;  
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, std::any_cast<uint32_t>(renderContext["width"]), std::any_cast<uint32_t>(renderContext["height"]));

        shader.mat4f("view", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["view"])));
        shader.mat4f("projection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["projection"])));
        shader.mat4f("lightSpace", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["lightSpace"])));
        
        shader.vec3f("viewPos", glm::value_ptr(std::any_cast<glm::vec3>(renderContext["viewPos"])));

        shader.veci("hasDirectionalLight", true);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["depthMap"])->bind("depthMap", 4, shader);
        shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));
        
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["voxels"])->bind("voxels", 7, shader);
        shader.veci("voxelDim", std::any_cast<int>(renderContext["voxelDim"]));
        shader.vecf("voxelGridSize", std::any_cast<float>(renderContext["voxelGridSize"]));
        shader.vecf("ALPHA_THRESH", alphaThresh);
        shader.vecf("MAX_DIST", maxDist);
        shader.veci("samples", samples);
        shader.vecf("tanHalfAngle", tanHalfAngles);

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
        pointLigthBuffer.bind(0);

        // texNoise->bind("texNoise", 9, shader);

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
        ImGui::DragFloat("tanHalfAngles", &tanHalfAngles, 0.01, 0, 100);
    }

private:
    float alphaThresh = .99;
    float maxDist = 1000;
    int samples = 1;
    float tanHalfAngles = 2.2;
    // std::shared_ptr<gfx::Texture> texNoise;
    gfx::Buffer pointLigthBuffer;

};

} // namespace renderer

#endif