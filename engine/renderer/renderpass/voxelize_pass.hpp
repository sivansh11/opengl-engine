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

        computeMipMap.addShader("../../../assets/shaders/voxels/generate_mip_maps.comp");
        computeMipMap.link();

        voxels = gfx::Texture::Builder{}
            .setMinFilter(gfx::Texture::MinFilter::eLINEAR_MIPMAP_LINEAR)
            .setMagFilter(gfx::Texture::MagFilter::eLINEAR)
            .setWrapR(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
            .setWrapS(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
            .setWrapT(gfx::Texture::Wrap::eCLAMP_TO_EDGE)
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

    void render(entt::registry& registry) override {
        renderContext->at("voxels") = voxels;
        renderContext->at("voxelGridSize") = voxelGridSize;
        renderContext->at("voxelDim") = voxelDimensions;

        // if (!revoxelize) return;
        // remakeVoxels();
        revoxelize = false;

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


        renderContext->at("depthMap").as<std::shared_ptr<gfx::Texture>>()->bind("depthMap", 3, shader);

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

        assert(renderContext->contains("lightSpace"));

        shader.mat4f("lightSpace", glm::value_ptr(renderContext->at("lightSpace").as<glm::mat4>()));

        voxels->bindImage("voxels", 5, 0, shader);

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

        timer.begin();
        for (auto [ent, model, transform] : registry.view<Model, core::TransformComponent>().each()) {
            model.draw(shader, transform);
        }
        timer.end();
        if (auto time = timer.popTimeStamp()) {
            voxelizationTime = time.value();
        } else {
            voxelizationTime = 0;
        }

        // voxels->genMipMaps();
        timer.begin();
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, std::string("Voxel Mip Mapping Pass").c_str());
        computeMipMap.bind(); 
        voxels->bind("voxelsDownSample", 0, computeMipMap);
        auto& voxelsInfo = voxels->getInfo();  
        int levels = std::log2(glm::max((glm::max(voxelsInfo.width, voxelsInfo.height)), voxelsInfo.depth)) + 1;
        for (int i = 1; i < levels; i++) {
            voxels->bindImage("voxels", 1, i, computeMipMap);
            glm::vec3 size{voxelsInfo.width / (1 << i), voxelsInfo.height / (1 << i), voxelsInfo.depth / (1 << i)};
            computeMipMap.veci("lod", i - 1);
            computeMipMap.dispatchCompute((size.x + 4 - 1) / 4, (size.y + 4 - 1) / 4, (size.z + 4 - 1) / 4);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        glPopDebugGroup();
        timer.end();
        if (auto time = timer.popTimeStamp()) {
            mipMappingTime = time.value();
        } else {
            mipMappingTime = 0;
        }


        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        renderContext->at("voxels") = voxels;
        renderContext->at("voxelGridSize") = voxelGridSize;
        renderContext->at("voxelDim") = voxelDimensions;
    }

    void UI() override {

        if (mipMappingTime != 0) {
            ImGui::Text("%s took %fms", "Mip Mapping", float(mipMappingTime / 1000000.0));
        } else {
            ImGui::Text("Timer information not availble!");
        }
        if (voxelizationTime != 0) {
            ImGui::Text("%s took %fms", "Voxelization", float(voxelizationTime / 1000000.0));
        } else {
            ImGui::Text("Timer information not availble!");
        }

        if (ImGui::Button("revoxelize")) { revoxelize = true; }
        if (ImGui::DragInt("Voxel Dims", &voxelDimensions, 1, 1, 512));
        if (ImGui::DragFloat("voxel grid size", &voxelGridSize, 1, 1, 1000));
    }

private:
    std::shared_ptr<gfx::Texture> voxels;
    float voxelGridSize = 30.f;
    int voxelDimensions = 512;
    glm::mat4 projX, projY, projZ;
    bool revoxelize = true;
    gfx::Buffer pointLigthBuffer;

    gfx::AsyncTimerQuery timer{5};
    float mipMappingTime;
    float voxelizationTime;

    gfx::ShaderProgram computeMipMap;

};

} // namespace renderer

#endif