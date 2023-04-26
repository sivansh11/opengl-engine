#ifndef RENDERER_SSAO_PASS_HPP
#define RENDERER_SSAO_PASS_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/framebuffer.hpp"
#include "../../gfx/buffer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <random>

namespace renderer {

class SSAOPass : public RenderPass {
public:
    SSAOPass(event::Dispatcher& dispatcher) : RenderPass("SSAO Pass", dispatcher) {
        shader.addShader("../../../assets/shaders/ssao/ssao.vert");
        shader.addShader("../../../assets/shaders/ssao/ssao.frag");
        shader.link();

        std::vector<glm::vec4> ssaoKernel;
        std::uniform_real_distribution<float> rn(0.0, 1.0); // random floats between [0.0, 1.0]
        std::default_random_engine g{0};

        for (int i = 0; i < numSamples; i++) {
            glm::vec3 sample = {
                rn(g) * 2.0f - 1.0f,
                rn(g) * 2.0f - 1.0f,
                rn(g)
            };
            sample = glm::normalize(sample);
            sample *= rn(g);
            float scale = float(i) / float(numSamples);

            auto lerp = [](float a, float b, float f) {
                return a + f * (b - a);
            };

            scale = lerp(.1f, 1.f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(glm::vec4{sample, 0});
        }

        ssaoKernelBuffer = gfx::Buffer(uint32_t(ssaoKernel.size() * sizeof(ssaoKernel[0])), gfx::Buffer::Useage::eSTATIC_DRAW, "samples");
        ssaoKernelBuffer.push(ssaoKernel.data());

        noiseTexture = gfx::Texture::Builder{}
            .setMinFilter(gfx::Texture::MinFilter::eNEAREST)
            .setMagFilter(gfx::Texture::MagFilter::eNEAREST)
            .setWrapS(gfx::Texture::Wrap::eREPEAT)
            .setWrapT(gfx::Texture::Wrap::eREPEAT)
            .build(gfx::Texture::Type::e2D, "texNoise");
        noiseTexture->loadImage("../../../assets/texture/noise.jpg");
    }

    ~SSAOPass() override {

    } 

    void render(entt::registry& registry) override {
        if (!enable) return; 

        renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->bind("texNormal", 1, shader);
        renderContext->at("texDepth").as<std::shared_ptr<gfx::Texture>>()->bind("texDepth", 5, shader);
        noiseTexture->bind("texNoise", 6, shader);
        
        shader.vecf("radius", radius);
        shader.vecf("bias", bias);
        shader.veci("numSamples", numSamples);
        shader.mat4f("projection", glm::value_ptr(renderContext->at("projection").as<glm::mat4>()));
        shader.mat4f("invProjection", glm::value_ptr(renderContext->at("invProjection").as<glm::mat4>()));
        shader.mat4f("view", glm::value_ptr(renderContext->at("view").as<glm::mat4>()));
        shader.mat4f("invView", glm::value_ptr(renderContext->at("invView").as<glm::mat4>()));
        ssaoKernelBuffer.bind(1);

        renderContext->at("frameBufferQuadVertexAttribute").as<std::shared_ptr<gfx::VertexAttribute>>()->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {
        ImGui::Checkbox("enable", &enable);
        ImGui::DragFloat("radius", &radius, 0.1, 0, 10000);
        ImGui::DragFloat("bias", &bias, 0.01, 0);
        if (ImGui::DragInt("numSamples)", &numSamples, 1, 1, 256)) {
            std::vector<glm::vec4> ssaoKernel;
            std::uniform_real_distribution<float> rn(0.0, 1.0); // random floats between [0.0, 1.0]
            std::default_random_engine g{0};

            for (int i = 0; i < numSamples; i++) {
                glm::vec3 sample = {
                    rn(g) * 2.0f - 1.0f,
                    rn(g) * 2.0f - 1.0f,
                    rn(g)
                };
                sample = glm::normalize(sample);
                sample *= rn(g);
                float scale = float(i) / float(numSamples);

                auto lerp = [](float a, float b, float f) {
                    return a + f * (b - a);
                };

                scale = lerp(.1f, 1.f, scale * scale);
                sample *= scale;
                ssaoKernel.push_back(glm::vec4{sample, 0});
            }
            ssaoKernelBuffer.resize(uint32_t(ssaoKernel.size() * sizeof(ssaoKernel[0])));
            ssaoKernelBuffer.push(ssaoKernel.data());
        }
    }

private:
    std::shared_ptr<gfx::Texture> noiseTexture;
    gfx::Buffer ssaoKernelBuffer;
    float radius = 0.5f;
    float bias = 0.025;
    int numSamples = 32;
    bool enable = true;
};

} // namespace renderer

#endif