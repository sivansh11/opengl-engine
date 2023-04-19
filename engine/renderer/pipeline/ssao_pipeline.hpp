#ifndef RENDERER_SSAO_PIPELINE_HPP
#define RENDERER_SSAO_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class SSAOPipeline : public BasePipeline {
public:
    SSAOPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "SSAO Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}.setSwizzleR(gfx::Texture::Swizzle::eR)
                                                  .setSwizzleG(gfx::Texture::Swizzle::eR)
                                                  .setSwizzleB(gfx::Texture::Swizzle::eR)
                                                  .setSwizzleA(gfx::Texture::Swizzle::eONE)
                                                  , gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eR32F, gfx::FrameBuffer::Attachment::eCOLOR0)
            .build();
    }

    ~SSAOPipeline() {

    }

    void preRender(entt::registry& registry, RenderContext& renderContext) override {
        if (halfRes) {
            frameBuffer.invalidate(std::any_cast<uint32_t>(renderContext["width"]) / 2, std::any_cast<uint32_t>(renderContext["height"]) / 2);
        } else {
            frameBuffer.invalidate(std::any_cast<uint32_t>(renderContext["width"]), std::any_cast<uint32_t>(renderContext["height"]));
        }
        frameBuffer.bind();
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR);
    }

    void postRender(entt::registry& registry, RenderContext& renderContext) override {
        frameBuffer.unbind();
        renderContext["ssaoImage"] = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        if (m_renderContextPtr->contains("ssaoImage"))
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(std::any_cast<std::shared_ptr<gfx::Texture>>(m_renderContextPtr->at("ssaoImage"))->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Checkbox("Half Res", &halfRes);
    }

private:
    gfx::FrameBuffer frameBuffer;
    bool halfRes = true;
};

} // namespace renderer

#endif