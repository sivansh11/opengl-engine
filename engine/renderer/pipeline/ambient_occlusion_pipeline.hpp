#ifndef RENDERER_AMBIENT_OCCLUSION_PIPELINE_HPP
#define RENDERER_AMBIENT_OCCLUSION_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class AmbientOcclusionPipeline : public BasePipeline {
public:
    AmbientOcclusionPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "AmbientOcclusion Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            // binding 4
            .addAttachment(gfx::Texture::Builder{}
                .setSwizzleR(gfx::Texture::Swizzle::eR)
                .setSwizzleG(gfx::Texture::Swizzle::eR)
                .setSwizzleB(gfx::Texture::Swizzle::eR)
                .setSwizzleA(gfx::Texture::Swizzle::eONE)
                , gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eR32F, gfx::FrameBuffer::Attachment::eCOLOR0)
            .build();
        
        frameBuffer.setClearColor(1, 1, 1, 1);
    }

    ~AmbientOcclusionPipeline() override {}

    void preRender(entt::registry& registry) override {
        if (halfRes) {
            frameBuffer.invalidate(renderContext->at("width").as<uint32_t>() / 2, renderContext->at("height").as<uint32_t>() / 2);        
        } else {
            frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());        
        }
        frameBuffer.bind(); 
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("texAmbientOcclusion") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("texAmbientOcclusion").as<std::shared_ptr<gfx::Texture>>()->getID())), {100, 100}, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Checkbox("Half Resolution", &halfRes);
    }

private:
    gfx::FrameBuffer frameBuffer;
    bool halfRes = false;
};

} // namespace renderer

#endif