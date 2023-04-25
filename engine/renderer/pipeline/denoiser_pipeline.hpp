#ifndef RENDERER_DENOISER_PIPELINE_HPP
#define RENDERER_DENOISER_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class DenoiserPipeline : public BasePipeline {
public:
    DenoiserPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Denoiser Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            // texDenoiser  binding 8
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            .build();
        frameBuffer.setClearColor(0, 0, 0, 0);
    }

    ~DenoiserPipeline() override {}

    void preRender(entt::registry& registry) override {
        renderContext->at("texDenoised") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texDenoised").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif