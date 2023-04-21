#ifndef RENDERER_MIP_MAP_PASS_HPP
#define RENDERER_MIP_MAP_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class MipMapPass : public RenderPass {
public:
    MipMapPass(event::Dispatcher& dispatcher) : RenderPass("MipMap Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/voxels/generate_mip_maps.comp");
        shader.link();

    }

    ~MipMapPass() override {

    } 

    void render(entt::registry& registry) override {
        auto voxels = renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>();
        voxels->bind("voxelsDownSample", 0, shader);
        auto& voxelsInfo = voxels->getInfo();  
        int levels = std::log2(glm::max((glm::max(voxelsInfo.width, voxelsInfo.height)), voxelsInfo.depth)) + 1;
        for (int i = 1; i < levels; i++) {
            voxels->bindImage("voxels", 0, i, shader);
            glm::vec3 size{voxelsInfo.width / (1 << i), voxelsInfo.height / (1 << i), voxelsInfo.depth / (1 << i)};
            shader.veci("lod", i - 1);
            shader.dispatchCompute((size.x + 4 - 1) / 4, (size.y + 4 - 1) / 4, (size.z + 4 - 1) / 4);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
    }

    void UI() override {

    }

private:

};

} // namespace renderer

#endif