#ifndef RENDERER_VOXEL_PIPELINE_HPP
#define RENDERER_VOXEL_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class VoxelPipeline : public BasePipeline {
public:
    VoxelPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Voxel Pipeline") {
        frameBuffer = gfx::FrameBuffer::Builder{800, 600}
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eRGBA8, gfx::FrameBuffer::Attachment::eCOLOR0)
            .addAttachment(gfx::Texture::Builder{}, gfx::Texture::Type::e2D, gfx::Texture::InternalFormat::eDEPTH_COMPONENT32, gfx::FrameBuffer::Attachment::eDEPTH)
            .build();
        frameBuffer.setClearColor(0, 0, 0, 0);
    }

    ~VoxelPipeline() {

    }

    void preRender(entt::registry& registry) override {
        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());
        frameBuffer.bind();
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("vxgiFinalImage") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("vxgiFinalImage").as<std::shared_ptr<gfx::Texture>>()->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
    }

private:
    gfx::FrameBuffer frameBuffer;
};

} // namespace renderer

#endif