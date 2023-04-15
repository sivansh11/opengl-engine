#ifndef RENDERER_BASIC_PIPELINE_HPP
#define RENDERER_BASIC_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class BasicPipeline : public BasePipeline {
public:
    BasicPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Basic Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();

        // m_dispatcher.subscribe<core::ViewPortResizeEvent>([this](const event::Event& event) {
        //     const core::ViewPortResizeEvent& e = reinterpret_cast<const core::ViewPortResizeEvent&>(event);
        //     this->frameBuffer.invalidate(e.width, e.height);
        // });
    }

    ~BasicPipeline() {

    }

    void preRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.invalidate(std::any_cast<uint32_t>(renderContext["width"]), std::any_cast<uint32_t>(renderContext["height"]));
        frameBuffer.bind();
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.unbind();
        renderContext["finalImage"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0)->getID();
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif