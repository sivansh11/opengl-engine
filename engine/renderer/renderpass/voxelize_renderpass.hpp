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
    void remakeVoxels() {
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
        createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA16F;
        createInfo.genMipMap = true;

        gfx::Texture::UploadInfo uploadInfo{};
        uploadInfo.dataType = gfx::Texture::DataType::eUNSIGNED_BYTE;
        uploadInfo.format = gfx::Texture::Format::eRGBA;
        uploadInfo.level = 0;
        uploadInfo.data = data;

        voxels->resize(createInfo);
        voxels->upload(uploadInfo);

        glm::mat4 projectionMatrix = glm::ortho(-voxelGridSize*0.5f, voxelGridSize*0.5f, -voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*1.5f);
        projX = projectionMatrix * glm::lookAt(glm::vec3(voxelGridSize, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projY = projectionMatrix * glm::lookAt(glm::vec3(0, voxelGridSize, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
        projZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, voxelGridSize), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    }

    VoxelizationPass(event::Dispatcher& dispatcher) : RenderPass("Voxelization Pass", dispatcher) {
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
        createInfo.internalFormat = gfx::Texture::InternalFormat::eRGBA16F;
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

        pointLigthBuffer = gfx::Buffer{gfx::Buffer::Useage::eDYNAMIC_DRAW};

    }

    ~VoxelizationPass() override {

    } 

    void render(entt::registry& registry, RenderContext& renderContext) override {
        renderContext["voxels"] = voxels;
        renderContext["voxelGridSize"] = voxelGridSize;
        renderContext["voxelDim"] = voxelDimensions;

        if (!revoxelize) return;
        revoxelize = false;

        remakeVoxels();

        bool has = false;
        core::DirectionalLightComponent dlc;
        for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
            dlc = dl;
            has = true;
            break;  
        }

        if (!has) return;

        glm::mat4 projectionMatrix = glm::ortho(-voxelGridSize*0.5f, voxelGridSize*0.5f, -voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*0.5f, voxelGridSize*1.5f);
        projX = projectionMatrix * glm::lookAt(glm::vec3(voxelGridSize, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projY = projectionMatrix * glm::lookAt(glm::vec3(0, voxelGridSize, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
        projZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, voxelGridSize), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


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

        shader.vec3f("directionalLight.position", glm::value_ptr(dlc.position));
        shader.vec3f("directionalLight.color", glm::value_ptr(dlc.color));
        shader.vec3f("directionalLight.ambience", glm::value_ptr(dlc.ambience));
        shader.vec3f("directionalLight.term", glm::value_ptr(dlc.term));

        assert(renderContext.contains("lightSpace"));

        shader.mat4f("lightSpace", glm::value_ptr(std::any_cast<glm::mat4>(renderContext["lightSpace"])));

        voxels->bindImage("voxels", 5, shader);

        std::vector<core::PointLightComponent> pointLights;
        auto pointLightEntities = registry.view<core::PointLightComponent>();
        for (auto [ent, pointLight] : pointLightEntities.each()) {
            pointLights.push_back(pointLight);
        }
        if (pointLights.size() > 0) {
            pointLigthBuffer.resize(pointLights.size() * sizeof(pointLights[0]));
            pointLigthBuffer.push(pointLights.data());
        } else {

        }
        shader.veci("numLights", pointLights.size());
        pointLigthBuffer.bind(0);

        for (auto [ent, model, transform] : registry.view<Model, core::TransformComponent>().each()) {
            model.draw(shader, transform);
        }

        voxels->genMipMaps();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        renderContext["voxels"] = voxels;
        renderContext["voxelGridSize"] = voxelGridSize;
        renderContext["voxelDim"] = voxelDimensions;
    }

    void UI() override {
        static bool once = false;
        if (!once) revoxelize = true;
        once = true;
        if (ImGui::Button("revoxelize")) { revoxelize = true; }
        if (ImGui::DragInt("Voxel Dims", &voxelDimensions, 1, 1, 512)) { revoxelize = true; }
        if (ImGui::DragFloat("voxel grid size", &voxelGridSize, 1, 1, 1000)) { revoxelize = true; }
    }

private:
    std::shared_ptr<gfx::Texture> voxels;
    float voxelGridSize = 150.f;
    int voxelDimensions = 256;
    glm::mat4 projX, projY, projZ;
    bool revoxelize = true;
    gfx::Buffer pointLigthBuffer;

};

} // namespace renderer

#endif