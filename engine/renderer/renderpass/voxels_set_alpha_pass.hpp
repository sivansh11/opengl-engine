#ifndef RENDERER_SET_ALPHA_PASS_HPP
#define RENDERER_SET_ALPHA_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include "../../core/events.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class SetAlphaPass : public RenderPass {
public:
    SetAlphaPass(event::Dispatcher& dispatcher) : RenderPass("SetAlpha Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/set_alpha.comp");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });
    }

    ~SetAlphaPass() override {} 

    void render(entt::registry& registry) override {

        if (!renderContext->at("voxelizeEveryFrame").as<bool>()) return;

        auto voxels = renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>();
        // voxels->bind("inVoxels", 0, shader);
        voxels->bindImage("voxels", 0, 0, shader);
        auto& voxelsInfo = voxels->getInfo();  
        shader.dispatchCompute((voxelsInfo.width + 4 - 1) / 4, (voxelsInfo.height + 4 - 1) / 4, (voxelsInfo.depth + 4 - 1) / 4);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    }

    void UI() override {

    }

private:

};

} // namespace renderer

#endif