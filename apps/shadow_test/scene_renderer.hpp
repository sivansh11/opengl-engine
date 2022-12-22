#ifndef SCENE_REDNERER_HPP
#define SCENE_REDNERER_HPP

#include "event_types.hpp"
#include "entity.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/buffer.hpp"
#include "gfx/vertex_attribute.hpp"
#include "gfx/shaders.hpp"

#include "model.hpp"
#include "components/camera.hpp"
#include "components/light.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstring>

class SceneRenderer {
public:
    SceneRenderer(event::Dispatcher& dispatcher) {

    }

    ~SceneRenderer() {

    }

    void render(core::Scene32& scene, core::EditorCamera& camera) {

    }

    GLuint displayViewPort() {
        return viewPort.getTextureID(GL_COLOR_ATTACHMENT0);
    }

private:
    core::gfx::FrameBuffer viewPort;

    core::gfx::Buffer frameBufferQuadBuffer;
    core::gfx::VertexAttribute frameBufferVertexAttribute;

    core::gfx::ShaderProgram geometryPass;
    core::gfx::ShaderProgram lightingPass;

    core::gfx::Buffer lightBuffer;
    std::vector<core::LightData> lights;

    core::LightData *mappedMemory;
};

#endif