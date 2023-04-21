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

        voxels = gfx::Texture::Builder{}
            .setMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_LINEAR)
            .setMagFilter(gfx::Texture::MagFilter::eLINEAR)
            .setWrapR(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
            .setWrapS(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
            .setWrapT(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
            .build(gfx::Texture::Type::e3D);

        gfx::Texture::CreateInfo createInfo{};
        createInfo.width = voxelDimensions;
        createInfo.height = voxelDimensions;
        createInfo.depth = voxelDimensions;
        createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA16F;

        voxels->create(createInfo);
    }

    ~VoxelPipeline() {

    }

    void preRender(entt::registry& registry) override {
        if (resize) {
            resize = false;
            gfx::Texture::CreateInfo createInfo{};
            createInfo.width = voxelDimensions;
            createInfo.height = voxelDimensions;
            createInfo.depth = voxelDimensions;
            createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA16F;
            voxels->resize(createInfo);   
        }

        frameBuffer.invalidate(renderContext->at("width").as<uint32_t>(), renderContext->at("height").as<uint32_t>());
        frameBuffer.bind();
        frameBuffer.clear(gfx::FrameBuffer::BufferBit::eCOLOR | gfx::FrameBuffer::BufferBit::eDEPTH);
    
        renderContext->at("voxels") = voxels;
        renderContext->at("voxelDimensions") = voxelDimensions;
        renderContext->at("voxelGridSize") = voxelGridSize;
    }

    void postRender(entt::registry& registry) override {
        frameBuffer.unbind();
        renderContext->at("vxgiFinalImage") = frameBuffer.getTexture(gfx::FrameBuffer::Attachment::eCOLOR0);
    }

    void pipelineUI() override {
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext->at("vxgiFinalImage").as<std::shared_ptr<gfx::Texture>>()->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::DragInt("Voxel Dims", &voxelDimensions, 1, 1, 512)) { resize = true; }
        ImGui::DragFloat("voxel grid size", &voxelGridSize, 1, 1, 1000);
    }

private:
    gfx::FrameBuffer frameBuffer;
    float voxelGridSize = 30.f;
    int voxelDimensions = 256;
    std::shared_ptr<gfx::Texture> voxels;
    bool resize = false;

};

} // namespace renderer

#endif