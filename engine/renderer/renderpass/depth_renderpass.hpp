#ifndef RENDERER_DEPTH_HPP
#define RENDERER_DEPTH_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class DepthPass : public RenderPass {
public:
    DepthPass() : RenderPass("Depth Pass") {
        shader.addShader("../../../assets/shaders/depth/depth.vert");
        shader.addShader("../../../assets/shaders/depth/depth.frag");
        shader.link();

        m_dlc.position = {0, 12, .1};
        m_dlc.color = {20, 20, 20};
        m_dlc.ambience = {.2, .2, .2};
        m_dlc.multiplier = 1;
        m_dlc.orthoProj = 15;
        m_dlc.far = 43;
        m_dlc.near = 0.1;
        m_dlc.term = {.3, .3, .1};
    }

    ~DepthPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        bool found = false;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dl = m_dlc;
            found = true;
            break;  
        }
        if (!found) {
            return;
        } else {
            // m_dlc = dlc;
        }

        glEnable(GL_DEPTH_TEST);
        glm::mat4 viewMatrix = glm::lookAt(glm::vec3(-0.3, 0.9, -0.25), glm::vec3(0,0,0), glm::vec3(0,1,0));
    	glm::mat4 projectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -500, 500);

        glm::mat4 lightSpaceMatrix = projectionMatrix * viewMatrix; 
        renderContext["lightSpaceMatrix"] = lightSpaceMatrix;
        
        shader.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        auto view = registry.view<Model, core::TransformComponent>();
        for (auto ent : view) {
            auto [model, transform] = registry.get<Model, core::TransformComponent>(ent);
            model.draw(shader, transform, false);
        }
    }

    void UI() override {
        // hack
        ImGui::DragFloat3("position", reinterpret_cast<float *>(&(m_dlc.position)));
        ImGui::DragFloat3("color", reinterpret_cast<float *>(&(m_dlc.color)));
        ImGui::DragFloat3("ambience", reinterpret_cast<float *>(&(m_dlc.ambience)));
        ImGui::DragFloat3("term", reinterpret_cast<float *>(&(m_dlc.term)));

        ImGui::DragFloat("multiplier", &m_dlc.multiplier);
        ImGui::DragFloat("orthoProj", &m_dlc.orthoProj);
        ImGui::DragFloat("far", &m_dlc.far);
        ImGui::DragFloat("near", &m_dlc.near);
    }

private:
    // hack
    core::DirectionalLightComponent m_dlc;
};

} // namespace renderer

#endif