#ifndef RENDERER_VISULIZATION_PIPELINE_HPP
#define RENDERER_VISULIZATION_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class VisulizationPipeline : public BasePipeline {
public:
    VisulizationPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Visulization Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
    }

    ~VisulizationPipeline() {

    }

    void preRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.invalidate(std::any_cast<uint32_t>(renderContext["width"]), std::any_cast<uint32_t>(renderContext["height"]));
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.unbind();
        renderContext["voxelVisual"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
   
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif