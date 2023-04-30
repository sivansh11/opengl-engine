#ifndef RENDERER_INDIRECT_LIGHTING_PIPELINE_HPP
#define RENDERER_INDIRECT_LIGHTING_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class IndirectLightPipeline : public BasePipeline {
public:
    IndirectLightPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "IndirectLight Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            // texIndirectLight  binding 7
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            .build();
        frameBuffer.setClearColor(0, 0, 0, 0);
    }

    ~IndirectLightPipeline() override {}

    void preRender(entt::registry& registry) override {
        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("texIndirectLight") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texIndirectLight").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif