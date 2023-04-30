#ifndef RENDERER_VXGI_IndirectLight_HPP
#define RENDERER_VXGI_IndirectLight_HPP

#include "../renderpass.hpp"

#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/framebuffer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../core/events.hpp"

namespace renderer {

class VXGIIndirectLightPass : public RenderPass {
public:
    VXGIIndirectLightPass(event::Dispatcher& dispatcher) : RenderPass("VXGIIndirectLight Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/vxgi/vxgi_indirect_lighting.vert");
        shader.addShader("../../../assets/shaders/vxgi/vxgi_indirect_lighting.frag");
        shader.link();

        dispatcher.subscribe<core::ReloadShaderEvent>([this](const event::Event& e) {
            this->shader.reload();
        });

        texNoise = gfx::Texture::Builder{}
            // .setSwizzleR(gfx::Texture::Swizzle::eR)
            // .setSwizzleG(gfx::Texture::Swizzle::eR)
            // .setSwizzleB(gfx::Texture::Swizzle::eR)
            // .setSwizzleA(gfx::Texture::Swizzle::eONE)
            .setWrapR(gfx::Texture::Wrap::eREPEAT)
            .setWrapS(gfx::Texture::Wrap::eREPEAT)
            .setWrapT(gfx::Texture::Wrap::eREPEAT)
            .build(gfx::Texture::Type::e2D);
        
        texNoise->loadImage("../../../assets/texture/bluenoise.png");

        texNoise->bind("texNoise", 8, shader);
    }

    ~VXGIIndirectLightPass() override {

    } 

    void render(entt::registry& registry) override {
        if (!enable) return; 
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // non settings data
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));

        auto& info = texNoise->getInfo();
        shader.vec2f("noiseUVScale", glm::value_ptr(glm::vec2{(float)renderContext->at("width").as<uint32_t>() / (float)info.width, (float)renderContext->at("height").as<uint32_t>() / (float)info.height}));
        renderContext->at("texAlbedoSpec").as<std::shared_ptr<gfx::Texture>>()->bind("texAlbedoSpec", 0, shader);
        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 1, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 5, shader);
        renderContext->at("voxels").as<std::shared_ptr<gfx::Texture>>()->bind("voxels", 6, shader);
        
        shader.vec3f("cameraPosition", glm::value_ptr(renderContext->at("cameraPosition").as<glm::vec3>()));

        // renderpass and pipeline settings data
        shader.veci("samples", samples);
        shader.vecf("MAX_DIST", maxDist);
        shader.vecf("ALPHA_THRESH", alphaThresh);
        shader.veci("MAX_COUNT", maxCount);
        shader.vecf("diffuseTanHalfAngle", diffuseTanHalfAngle);
        shader.vecf("specularTanHalfAngle", specularTanHalfAngle);
        shader.veci("voxelDimensions", renderContext->at("voxelDimensions").as<int>());
        shader.vecf("voxelGridSize", renderContext->at("voxelGridSize").as<float>());
        shader.vecf("randSeed", ((float)(rand()) / (float)(RAND_MAX)) * rand());

        renderContext->at("frameBufferQuadVertexAttribute").as<std::shared_ptr<gfx::VertexAttribute>>()->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {
        ImGui::Checkbox("enable", &enable);
        ImGui::DragFloat("AlphaThresh", &alphaThresh, .001, .01, .99);
        ImGui::DragFloat("MaxDist", &maxDist, 1, 0, 5000);
        ImGui::DragFloat("diffuseTanHalfAngle", &diffuseTanHalfAngle, 0.01, 0, 100);
        ImGui::DragFloat("specularTanHalfAngle", &specularTanHalfAngle, 0.01, 0, 100);
        ImGui::DragInt("samples", &samples, 1, 1, 15);
        ImGui::DragInt("maxCount", &maxCount, 1, 1, 30);
    }

private:
    float alphaThresh = .99;
    float maxDist = 1000000000;
    int samples = 4;
    int maxCount = 1000;
    float diffuseTanHalfAngle = .32;
    float specularTanHalfAngle = .07;
    std::shared_ptr<gfx::Texture> texNoise;
    bool enable = true;
};

} // namespace renderer

#endif