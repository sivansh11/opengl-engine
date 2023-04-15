#ifndef RENDERER_DEFERRED_PIPELINE_HPP
#define RENDERER_DEFERRED_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class DeferredPipeline : public BasePipeline {
public:
    DeferredPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Deferred Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            // albedospec
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            // normal
            .addAttachment(gfx::Texture::Builder{}.setSwizzleA(gfx::Texture::Swizzle::eONE), gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA16F, gfx::FrameBuffer::Attachment::eCOLOR1)
            // position
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA16F, gfx::FrameBuffer::Attachment::eCOLOR2)
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
    }

    ~DeferredPipeline() {

    }

    void preRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.invalidate(std::any_cast<uint32_t>(renderContext["width"]), std::any_cast<uint32_t>(renderContext["height"]));
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.unbind();
        renderContext["texAlbedoSpec"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
        renderContext["texPosition"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR2);
        renderContext["texNormal"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR1);
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif