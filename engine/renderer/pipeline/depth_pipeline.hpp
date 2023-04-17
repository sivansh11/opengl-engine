#ifndef RENDERER_DEPTH_PIPELINE_HPP
#define RENDERER_DEPTH_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class DepthPipeline : public BasePipeline {
public:
    DepthPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Depth Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
        frameBuffer.setClearDepth(1);
        // m_dispatcher.subscribe<core::ViewPortResizeEvent>([this](const event::Event& event) {
        //     const core::ViewPortResizeEvent& e = reinterpret_cast<const core::ViewPortResizeEvent&>(event);
        //     this->frameBuffer.invalidate(e.width, e.height);
        // });
    }

    ~DepthPipeline() {

    }

    void preRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.invalidate(std::any_cast<uint32_t>(renderContext["width"]), std::any_cast<uint32_t>(renderContext["height"]));
        frameBuffer.bind();
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.unbind();
        renderContext["depthImage"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eDEPTH);
    }
    
    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(std::any_cast<std::shared_ptr<gfx::Texture>>(m_renderContextPtr->at("depthImage"))->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    }
    
private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif