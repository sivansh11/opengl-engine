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
    SSAOPass() : RenderPass("SSAO Pass") {
        shader.addShader("../../../assets/shaders/ssao/ssao.vert");
        shader.addShader("../../../assets/shaders/ssao/ssao.frag");
        shader.link();

        vertexBuffer = gfx::Buffer{static_cast<uint32_t>(gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0]))};
        vertexBuffer.push(gfx::frameBufferQuadVertices.data());
        vao.attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
        vao.attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
        vao.bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);
    
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
            .build(gfx::Texture::Type::e2D, "Noise Texture");
        noiseTexture->loadImage("../../../assets/texture/noise.jpg");

        // std::vector<glm::vec4> ssaoNoise;
        // int noiseWidth = 100;
        // int noiseHeight = 100;
        // for (int i = 0; i < noiseWidth; i++) for (int j = 0; j < noiseHeight; j++) {
        //     glm::vec4 noise = {
        //         rn(g) * 2.0f - 1.0f,
        //         rn(g) * 2.0f - 1.0f,
        //         0.f,
        //         0.f
        //     };
        //     ssaoNoise.push_back(noise);
        // }

        // gfx::Texture::CreateInfo createInfo{};
        // createInfo.width = noiseWidth;
        // createInfo.height = noiseHeight;
        // createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA16F;
        // createInfo.genMipMap = true;
        // noiseTexture->create(createInfo);

        // gfx::Texture::UploadInfo uploadInfo{};
        // uploadInfo.data = ssaoNoise.data();
        // uploadInfo.dataType = gfx::Texture::DataType::eFLOAT;
        // uploadInfo.format = gfx::Texture::Format::eRGBA;
        // noiseTexture->upload(uploadInfo);
    }

    ~SSAOPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texPosition"])->bind("texPosition", 1, shader);
        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["texNormal"])->bind("texNormal", 2, shader);
        shader.vecf("radius", radius);
        shader.vecf("bias", bias);
        shader.veci("numSamples", numSamples);
        shader.mat4f("projection", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["projection"])));
        shader.mat4f("view", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["view"])));
        ssaoKernelBuffer.bind(0);
        noiseTexture->bind("texNoise", 3, shader);
        vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void UI() override {
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
    gfx::VertexAttribute vao;
    std::shared_ptr<gfx::Texture> noiseTexture;
    gfx::Buffer vertexBuffer;
    gfx::Buffer ssaoKernelBuffer;
    float radius = 0.5f;
    float bias = 0.025;
    int numSamples = 32;
};

} // namespace renderer

#endif