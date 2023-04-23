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
            // albedospec  binding 0
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            // normal      binding 1
            .addAttachment(gfx::Texture::Builder{}.setSwizzleA(gfx::Texture::Swizzle::eONE), gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA16F, gfx::FrameBuffer::Attachment::eCOLOR1)
            // tangent   binding 8
            .addAttachment(gfx::Texture::Builder{}.setSwizzleA(gfx::Texture::Swizzle::eONE), gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA16F, gfx::FrameBuffer::Attachment::eCOLOR2)
            // depth       binding 2
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
        frameBuffer.setClearDepth(1);
    }

    ~DeferredPipeline() {

    }

    void preRender(entt::registry& registry) override {
        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("texAlbedoSpec") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
        renderContext->at("texNormal") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR1);
        renderContext->at("texTangent") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR2);
        renderContext->at("texDepth") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eDEPTH);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texAlbedoSpec").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texNormal").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texTangent").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif