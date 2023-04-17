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
    }

    ~DepthPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            break;  
        }

        glEnable(GL_DEPTH_TEST);
        glm::mat4 lightProjection = glm::ortho(-dlc.orthoProj, dlc.orthoProj, -dlc.orthoProj, dlc.orthoProj, dlc.near, dlc.far);  
        glm::mat4 lightView = glm::lookAt(dlc.position * dlc.multiplier, 
                                  glm::vec3( 0.0f, 0.0f,  0.0f), 
                                  glm::vec3( 0.0f, 1.0f,  0.0f));  
        glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
        shader.mat4f("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        auto view = registry.view<Model>();
        for (auto ent : view) {
            auto& model = registry.get<Model>(ent);
            model.draw(shader, {}, false);
        }
    }

private:

};

} // namespace renderer

#endif