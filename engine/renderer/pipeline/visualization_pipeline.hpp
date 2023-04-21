#ifndef RENDERER_VISULIZATION_PIPELINE_HPP
#define RENDERER_VISULIZATION_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class VisualizationPipeline : public BasePipeline {
public:
    VisualizationPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Visualization Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
    }

    ~VisualizationPipeline() {

    }

    void preRender(entt::registry& registry) override {
        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("voxelVisual") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
   
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif