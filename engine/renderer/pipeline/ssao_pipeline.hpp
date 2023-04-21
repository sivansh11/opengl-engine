#ifndef RENDERER_SSAO_PIPELINE_HPP
#define RENDERER_SSAO_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class SSAOPipeline : public BasePipeline {
public:
    SSAOPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "SSAO Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}.setSwizzleR(gfx::Texture::Swizzle::eR)
                                                  .setSwizzleG(gfx::Texture::Swizzle::eR)
                                                  .setSwizzleB(gfx::Texture::Swizzle::eR)
                                                  .setSwizzleA(gfx::Texture::Swizzle::eONE)
                                                  , gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eR32F, gfx::FrameBuffer::Attachment::eCOLOR0)
            .build();
    }

    ~SSAOPipeline() {

    }

    void preRender(entt::registry& registry) override {
        if (halfRes) {
            frameBuffer.invalidate(renderContext->at("width").as<uint32_t>() / 2, renderContext->at("height").as<uint32_t>() / 2);        
        } else {
            frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        }
        frameBuffer.bind();
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("ssaoImage") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        if (renderContext->contains("ssaoImage")) {
            ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("ssaoImage").as<std::shared_ptr<gfx::Texture>>()->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Checkbox("Half Res", &halfRes);
        }
    }

private:
    gfx::FrameBuffer frameBuffer;
    bool halfRes = true;
};

} // namespace renderer

#endif