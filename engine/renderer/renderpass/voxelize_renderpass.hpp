#ifndef RENDERER_VOXELIZE_HPP
#define RENDERER_VOXELIZE_HPP

#include "../renderpass.hpp"
#include "../model.hpp"

#include "../../gfx/shaders.hpp"
#include "../../gfx/buffer.hpp"
#include "../../gfx/texture.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace renderer {

class VoxelizationPass : public RenderPass {
public:
    VoxelizationPass() : RenderPass("Voxelization Pass") {
        shader.addShader("../../../assets/shaders/voxels/voxelization.vert");
        shader.addShader("../../../assets/shaders/voxels/voxelization.geom");
        shader.addShader("../../../assets/shaders/voxels/voxelization.frag");
        shader.link();

        voxels = gfx::Texture::Builder{}
            .setMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_LINEAR)
            .setMagFilter(gfx::Texture::MagFilter::eLINEAR)
            .build(gfx::Texture::Type::e3D);

        char *data = new char[voxelDimensions * voxelDimensions * voxelDimensions * 4];
        
        for (int i = 0; i < voxelDimensions; i++) {
            for (int j = 0; j < voxelDimensions; j++) {
                for (int k = 0; k < voxelDimensions; k++) {
                    data[4*(i + j * voxelDimensions + k * voxelDimensions * voxelDimensions) + 0] = 0;
                    data[4*(i + j * voxelDimensions + k * voxelDimensions * voxelDimensions) + 1] = 0;
                    data[4*(i + j * voxelDimensions + k * voxelDimensions * voxelDimensions) + 2] = 0;
                    data[4*(i + j * voxelDimensions + k * voxelDimensions * voxelDimensions) + 3] = 0;
                }
            }
        }

        gfx::Texture::CreateInfo createInfo{};
        createInfo.width = voxelDimensions;
        createInfo.height = voxelDimensions;
        createInfo.depth = voxelDimensions;
        createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA8;
        createInfo.genMipMap = true;

        gfx::Texture::UploadInfo uploadInfo{};
        uploadInfo.dataType = gfx::Texture::DataType::eUNSIGNED_BYTE;
        uploadInfo.format = gfx::Texture::Format::eRGBA;
        uploadInfo.level = 0;
        uploadInfo.data = data;

        voxels->create(createInfo);
        voxels->upload(uploadInfo);

        glm::mat4 projectionMatrix = glm::ortho(-voxelGridSize*0.5f, voxelGridSize*0.5f, -voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*1.5f);
        projX = projectionMatrix * glm::lookAt(glm::vec3(voxelGridSize, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projY = projectionMatrix * glm::lookAt(glm::vec3(0, voxelGridSize, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
        projZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, voxelGridSize), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }

    ~VoxelizationPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        bool has = false;
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            has = true;
            break;  
        }

        if (!has) return;

        std::any_cast<std::shared_ptr<gfx::Texture>>(renderContext["depthMap"])->bind("depthMap", 3, shader);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glViewport(0, 0, voxelDimensions, voxelDimensions);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.veci("voxelDim", voxelDimensions);
        shader.mat4f("projX", glm::value_ptr(projX));
        shader.mat4f("projY", glm::value_ptr(projY));
        shader.mat4f("projZ", glm::value_ptr(projZ));

    	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(-0.3, 0.9, -0.25), glm::vec3(0,0,0), glm::vec3(0,1,0));
    	glm::mat4 projectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -500, 500);

        glm::mat4 lightSpace = projectionMatrix * viewMatrix;

        // shader.mat4f("view", glm::value_ptr(viewMatrix));
        // shader.mat4f("projection", glm::value_ptr(projectionMatrix));

        shader.mat4f("lightSpace", glm::value_ptr(lightSpace));

        voxels->bindImage("voxels", 5, shader);

        for (auto [ent, model, transform] : registry.view<Model, core::TransformComponent>().each()) {
            model.draw(shader, transform);
        }

        voxels->genMipMaps();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        renderContext["voxels"] = voxels;
    }

    void UI() override {

    }

private:
    std::shared_ptr<gfx::Texture> voxels;
    float voxelGridSize = 75.f;
    int voxelDimensions = 256;
    glm::mat4 projX, projY, projZ;
};

} // namespace renderer

#endif