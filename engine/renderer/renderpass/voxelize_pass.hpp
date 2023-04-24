#ifndef RENDERER_VOXELIZE_HPP
#define RENDERER_VOXELIZE_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/texture.hpp"

#include "../../core/events.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class VoxelizationPass : public RenderPass {
public:
    VoxelizationPass(event::Dispatcher& dispatcher) : RenderPass("Voxelization Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/voxelization.vert");
        shader.addShader("../../../assets/shaders/voxels/voxelization.geom");
        shader.addShader("../../../assets/shaders/voxels/voxelization.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~VoxelizationPass() override {

    } 

    void render(entt::registry& registry) override {
        if (!renderContext->at("voxelizeEveryFrame").as<bool>()) return;
        
        float voxelGridSize = renderContext->at("voxelGridSize").as<float>();
        int voxelDimensions = renderContext->at("voxelDimensions").as<int>();
        auto voxels = renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>();

        glm::mat4 projectionMatrix = glm::ortho(-voxelGridSize*0.5f, voxelGridSize*0.5f, -voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*1.5f);
        glm::mat4 projX = projectionMatrix * glm::lookAt(glm::vec3(voxelGridSize, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 projY = projectionMatrix * glm::lookAt(glm::vec3(0, voxelGridSize, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
        glm::mat4 projZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, voxelGridSize), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        renderContext->at("texShadow").as<std::shared_ptr<gfx::Texture>>()->bind("texShadow", 3, shader);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glViewport(0, 0, voxelDimensions, voxelDimensions);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.veci("voxelDimensions", voxelDimensions);
        shader.mat4f("projX", glm::value_ptr(projX));
        shader.mat4f("projY", glm::value_ptr(projY));
        shader.mat4f("projZ", glm::value_ptr(projZ));

        auto& dlc = renderContext->at("directionalLight").as<core::DirectionalLightComponent>();
        shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));

        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));

        voxels->bindImage("voxels", 6, 0, shader);

        renderContext->at("pointLightBuffer").as<std::shared_ptr<gfx::Buffer>>()->bind(0);
        shader.veci("numPointLights", renderContext->at("numPointLights").as<int>());

        for (auto [ent, model, transform] : registry.view<Model, core::TransformComponent>().each()) {
            model.draw(shader, transform);
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());
    }

    void UI() override {
        
    }

private:    

};

} // namespace renderer

#endif