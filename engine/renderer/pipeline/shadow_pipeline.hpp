#ifndef RENDERER_SHADOW_PIPELINE_HPP
#define RENDERER_SHADOW_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class ShadowPipeline : public BasePipeline {
public:
    ShadowPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Shadow Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            // binding 3
            .addAttachment(gfx::Texture::Builder{}
                .setMagFilter(gfx::Texture::MagFilter::eLINEAR)
                .setMinFilter(gfx::Texture::MinFilter::eLINEAR)
                .setWrapR(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
                .setWrapS(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
                .setSwizzleR(gfx::Texture::Swizzle::eR)
                .setSwizzleG(gfx::Texture::Swizzle::eR)
                .setSwizzleB(gfx::Texture::Swizzle::eR)
                .setSwizzleA(gfx::Texture::Swizzle::eONE)
                .setCompareFunc(gfx::Texture::CompareFunc::eLEQUAL), gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
        
        dimensions[0] = 1024 * 4;
        dimensions[1] = 1024 * 4;
    }

    ~ShadowPipeline() override {}

    void preRender(entt::registry& registry) override {
        frameBuffer.invalidate(dimensions[0], dimensions[1]);        
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("texShadow") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eDEPTH);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texShadow").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::DragFloat2("Shadow Map Dimensions", dimensions);
    }

private:
    gfx::FrameBuffer frameBuffer;
    float dimensions[2];
};

} // namespace renderer

#endif