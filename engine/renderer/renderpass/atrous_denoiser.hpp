#ifndef RENDERER_ATROUS_DENOISER_HPP
#define RENDERER_ATROUS_DENOISER_HPP

#include "../renderpass.hpp"

#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

#include "../../gfx/timer.hpp"

namespace renderer {

class ATrousDenoiserPass : public RenderPass {
public:
    ATrousDenoiserPass(event::Dispatcher& dispatcher) : RenderPass("ATrousDenoiser Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/denoiser/atrous.vert");
        shader.addShader("../../../assets/shaders/denoiser/atrous.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~ATrousDenoiserPass() override {} 

    void render(entt::registry& registry) override {
        if (disable) return;
        renderContext->at("texIndirectLight").as<std::shared_ptr<gfx::Texture>>()->bind("texIndirectLight", 7, shader);
        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 1, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 5, shader);

        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));

        shader.veci("width", renderContext->at("width").as<uint32_t>());
        shader.veci("height", renderContext->at("height").as<uint32_t>());

        auto texIndirectLight = renderContext->at("texIndirectLight").as<std::shared_ptr<gfx::Texture>>();
        int width = texIndirectLight->getInfo().width;
        int height = texIndirectLight->getInfo().height;

        for (int i = 0; i < filter; i++) {
            shader.vecf("cPhi", cPhi0 * pow(2, -i));
            shader.vecf("nPhi", nPhi0 * pow(2, -i));
            shader.vecf("pPhi", pPhi0 * pow(2, -i));
            shader.veci("stepWidth", pow(2, i) + 1);

            renderContext->at("frameBufferQuadVertexAttribute").as<std::shared_ptr<gfx::VertexAttribute>>()->bind();

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // -> texDenoised
            glCopyImageSubData(renderContext->at("texDenoised").as<std::shared_ptr<gfx::Texture>>()->getID(), GL_TEXTURE_2D, 0, 0, 0, 0, 
                               renderContext->at("texIndirectLight").as<std::shared_ptr<gfx::Texture>>()->getID(), GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);


        }
    }

    void UI() override {
        ImGui::Checkbox("Disable", &disable);
        ImGui::DragFloat("cPhi0", &cPhi0);
        ImGui::DragFloat("nPhi0", &nPhi0);
        ImGui::DragFloat("pPhi0", &pPhi0);
        ImGui::DragInt("Iterations", &filter, 1, 1, 15);
    }

private:
    int filter = 3;
    float cPhi0 = 20.4f;
    float nPhi0 = 1E-2f;
    float pPhi0 = 1E-1f;
    bool disable = false;
};

} // namespace renderer

#endif