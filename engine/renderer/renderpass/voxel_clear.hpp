#ifndef RENDERER_CLEAR_PASS_HPP
#define RENDERER_CLEAR_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class ClearPass : public RenderPass {
public:
    ClearPass(event::Dispatcher& dispatcher) : RenderPass("Clear Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/clear_voxels.comp");
        shader.link();

    }

    ~ClearPass() override {} 

    void render(entt::registry& registry) override {

        if (!renderContext->at("voxelizeEveryFrame").as<bool>()) return;

        auto voxels = renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>();
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