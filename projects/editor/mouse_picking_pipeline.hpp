#ifndef MOUSE_PICKING_PIPELINE_HPP
#define MOUSE_PICKING_PIPELINE_HPP

#include "renderer/pipeline.hpp"

#include "core/events.hpp"

#include "gfx/framebuffer.hpp"

class MousePickingPipeline : public renderer::BasePipeline {
public:
    MousePickingPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "MousePicking Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}
                .setSwizzleR(gfx::Texture::Swizzle::eR)
                .setSwizzleG(gfx::Texture::Swizzle::eR)
                .setSwizzleB(gfx::Texture::Swizzle::eR)
                .setSwizzleA(gfx::Texture::Swizzle::eONE)
                , gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eR32I, gfx::FrameBuffer::Attachment::eCOLOR0)
            .addAttachment(gfx::Texture::Builder{}
                .setSwizzleR(gfx::Texture::Swizzle::eR)
                .setSwizzleG(gfx::Texture::Swizzle::eR)
                .setSwizzleB(gfx::Texture::Swizzle::eR)
                .setSwizzleA(gfx::Texture::Swizzle::eONE)
                , gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
        frameBuffer.setClearColor(0, 0, 0, 0);
    }

    ~MousePickingPipeline() override {}

    void preRender(entt::registry& registry) override {
        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("texMousePicking") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texMousePicking").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
    }

private:
    gfx::FrameBuffer frameBuffer;
};

#endif