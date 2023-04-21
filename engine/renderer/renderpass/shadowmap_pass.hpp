#ifndef RENDERER_DEPTH_HPP
#define RENDERER_DEPTH_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class ShadowMapPass : public RenderPass {
public:
    ShadowMapPass(event::Dispatcher& dispatcher) : RenderPass("ShadowMap Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/depth/depth.vert");
        shader.addShader("../../../assets/shaders/depth/depth.frag");
        shader.link();

        m_dlc.position = {-.01, 12, 0};
        m_dlc.color = {5, 5, 5};
        m_dlc.ambience = {.3, .3, .3};
        m_dlc.multiplier = 2;
        m_dlc.orthoProj = 15;
        m_dlc.far = 43;
        m_dlc.near = 0.1;
        m_dlc.term = {.3, .3, .1};
    }

    ~ShadowMapPass() override {

    } 

    void render(entt::registry& registry) override {
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
        // glm::mat4 viewMatrix = glm::lookAt(glm::vec3(-0.3, 0.9, -0.25), glm::vec3(0,0,0), glm::vec3(0,1,0));
    	// glm::mat4 projectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -500, 500);

        glm::mat4 lightProjection = glm::ortho(-m_dlc.orthoProj, m_dlc.orthoProj, -m_dlc.orthoProj, m_dlc.orthoProj, m_dlc.near, m_dlc.far);  
        glm::mat4 lightView = glm::lookAt(m_dlc.position * m_dlc.multiplier, 
                                  glm::vec3( 0.0f, 0.0f,  0.0f), 
                                  glm::vec3( 0.0f, 1.0f,  0.0f));  

        glm::mat4 lightSpace = lightProjection * lightView; 
        renderContext->at("lightSpace") = lightSpace;
        
        shader.mat4f("lightSpace", glm::value_ptr(lightSpace));
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