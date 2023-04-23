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

    }

    ~ShadowMapPass() override {

    } 

    void render(entt::registry& registry) override {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        core::DirectionalLightComponent dlc;
        bool found = false;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
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

        glm::mat4 lightProjection = glm::ortho(-dlc.orthoProj, dlc.orthoProj, -dlc.orthoProj, dlc.orthoProj, dlc.near, dlc.far);  
        glm::mat4 lightView = glm::lookAt(dlc.position * dlc.multiplier, 
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
        

        
    }

private:
    // hack
    core::DirectionalLightComponent m_dlc;
};

} // namespace renderer

#endif