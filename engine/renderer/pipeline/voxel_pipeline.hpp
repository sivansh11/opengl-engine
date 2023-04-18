#ifndef RENDERER_VOXEL_PIPELINE_HPP
#define RENDERER_VOXEL_PIPELINE_HPP

#include "../pipeline.hpp"

#include "../../core/events.hpp"

#include "../../gfx/framebuffer.hpp"

namespace renderer {

class VoxelPipeline : public BasePipeline {
public:
    VoxelPipeline(event::Dispatcher& dispatcher) : BasePipeline(dispatcher, "Voxel Pipeline") {
        
    }

    ~VoxelPipeline() {

    }

    void preRender(entt::registry& registry, RenderContext& renderContext) override {
        
    }

    void postRender(entt::registry& registry, RenderContext& renderContext) override {
        
    }

    void pipelineUI() override {
        
    }

private:

};

} // namespace renderer

#endif