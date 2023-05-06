#include "app.hpp"

#include "core/imgui_utils.hpp"

#include "renderer/pipeline/deferred_pipeline.hpp"
#include "renderer/renderpass/geometry_pass.hpp"
#include "renderer/pipeline/shadow_pipeline.hpp"
#include "renderer/renderpass/shadow_pass.hpp"
#include "renderer/pipeline/ambient_occlusion_pipeline.hpp"
#include "renderer/renderpass/ssao_pass.hpp"
#include "renderer/pipeline/composite_pipeline.hpp"
#include "renderer/renderpass/composite_pass.hpp"
#include "renderer/pipeline/voxel_pipeline.hpp"
#include "renderer/renderpass/voxel_clear.hpp"
#include "renderer/renderpass/voxelize_pass.hpp"
#include "renderer/renderpass/voxels_set_alpha_pass.hpp"
#include "renderer/renderpass/voxel_mipmap_pass.hpp"
#include "renderer/renderpass/voxel_visualize_pass.hpp"
#include "renderer/pipeline/indirect_lighing_pipeline.hpp"
#include "renderer/renderpass/vxgi_indirect_lighting_pass.hpp"
#include "renderer/pipeline/denoiser_pipeline.hpp"
#include "renderer/renderpass/atrous_denoiser.hpp"

#include "mouse_picking_pipeline.hpp"
#include "mouse_picking_pass.hpp"

#include "testing_panel.hpp"

#include "core/event.hpp"

#include "core/components.hpp"

#include "editor_camera.hpp"

#include "gfx/framebuffer.hpp"
#include "gfx/timer.hpp"

#include <entt/entt.hpp>

#include <iostream>
#include <chrono>
#include <thread>

#define ASSETS_PATH std::string("../../../assets")

App::App() : window("Editor", 800, 600) {
    core::initImgui(window);
    // core::setupImGuiStyle();
    // core::myDefaultImGuiStyle();
}

App::~App() {
    core::terminateImgui();
}

void App::run() {

    entt::registry registry;
    event::Dispatcher dispatcher;

    EditorCamera camera;
    uint32_t width = 100, height = 100;

    // pipeline and renderpasses
    renderer::DeferredPipeline deferredPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> geometryPass = std::make_shared<renderer::GeometryPass>(dispatcher);
    deferredPipeline.addRenderPass(geometryPass);

    renderer::ShadowPipeline shadowPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> shadowPass = std::make_shared<renderer::ShadowPass>(dispatcher);
    shadowPipeline.addRenderPass(shadowPass);

    renderer::VoxelPipeline voxelPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> voxelClearPass = std::make_shared<renderer::ClearPass>(dispatcher);
    std::shared_ptr<renderer::RenderPass> voxelizePass = std::make_shared<renderer::VoxelizationPass>(dispatcher);
    std::shared_ptr<renderer::RenderPass> voxelSetAlphaPass = std::make_shared<renderer::SetAlphaPass>(dispatcher);
    std::shared_ptr<renderer::RenderPass> voxelMipMapPass = std::make_shared<renderer::MipMapPass>(dispatcher);
    std::shared_ptr<renderer::RenderPass> voxelVisualizePass = std::make_shared<renderer::VisualizePass>(dispatcher);
    voxelPipeline.addRenderPass(voxelClearPass);
    voxelPipeline.addRenderPass(voxelizePass);
    voxelPipeline.addRenderPass(voxelSetAlphaPass);
    voxelPipeline.addRenderPass(voxelMipMapPass);
    voxelPipeline.addRenderPass(voxelVisualizePass);

    renderer::AmbientOcclusionPipeline ambientOcclusionPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> ssaoPass = std::make_shared<renderer::SSAOPass>(dispatcher);
    ambientOcclusionPipeline.addRenderPass(ssaoPass);

    renderer::IndirectLightPipeline indirectPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> vxgiPass = std::make_shared<renderer::VXGIIndirectLightPass>(dispatcher);
    indirectPipeline.addRenderPass(vxgiPass);

    renderer::DenoiserPipeline denoiserPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> denoiserPass = std::make_shared<renderer::ATrousDenoiserPass>(dispatcher);
    denoiserPipeline.addRenderPass(denoiserPass);

    renderer::CompositePipeline compositePipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> compositePass = std::make_shared<renderer::CompositePass>(dispatcher);
    compositePipeline.addRenderPass(compositePass);

    MousePickingPipeline mousePickingPipeline{dispatcher};
    std::shared_ptr<renderer::RenderPass> mousePickingPass = std::make_shared<MousePickingPass>(dispatcher);
    mousePickingPipeline.addRenderPass(mousePickingPass);

    // helper
    std::vector<core::BasePanel *> pipelines;
    pipelines.push_back(&mousePickingPipeline);
    pipelines.push_back(&deferredPipeline);
    pipelines.push_back(&shadowPipeline);
    pipelines.push_back(&voxelPipeline);
    pipelines.push_back(&indirectPipeline);
    pipelines.push_back(&denoiserPipeline);
    pipelines.push_back(&ambientOcclusionPipeline);
    pipelines.push_back(&compositePipeline);

    {
        renderer::Model model;
        model.loadModelFromPath("../../../assets/Sponza/glTF/Sponza.gltf");
        for (auto mesh : model.m_meshes) {
            auto ent = registry.create();
            registry.emplace<std::shared_ptr<renderer::Mesh>>(ent) = mesh;
        }
    }

    {
        
    }

    auto ent = registry.create();
    auto& pl = registry.emplace<core::PointLightComponent>(ent);
    pl.position = {6, 1, 0};
    pl.color = {0, 0, 0};
    pl.term = {.3, .3, .1};

    ent = registry.create();
    auto& dlc = registry.emplace<core::DirectionalLightComponent>(ent);
    dlc.position = {.01, 12, 6};
    dlc.color = {100, 100, 100};
    dlc.ambience = {.0, .0, .0};
    dlc.term = {.3, .3, .1};
    dlc.multiplier = 2;
    dlc.orthoProj = 15;
    dlc.far = 43;
    dlc.near = 0.1;
    
    ViewPanel viewPanel;
    viewPanel.addItem("texComposite");
    viewPanel.addItem("texIndirectLight");
    viewPanel.addItem("texDenoised");
    viewPanel.addItem("texAmbientOcclusion");
    viewPanel.addItem("texVisualizeVoxel");
    viewPanel.addItem("texAlbedoSpec");
    viewPanel.addItem("texNormal");
    viewPanel.addItem("texDepth");
    viewPanel.addItem("texShadow");

    std::vector<core::BasePanel *> panels;
    panels.push_back(&viewPanel);

    std::shared_ptr<gfx::Buffer> pointLigthBuffer = std::make_shared<gfx::Buffer>(gfx::Buffer::Useage::eDYNAMIC_DRAW);
    
    std::shared_ptr<gfx::VertexAttribute> frameBufferQuadVertexAttribute = std::make_shared<gfx::VertexAttribute>();
    gfx::Buffer vertexBuffer = gfx::Buffer{static_cast<uint32_t>(gfx::frameBufferQuadVertices.size() * sizeof(gfx::frameBufferQuadVertices[0])), gfx::Buffer::Useage::eDYNAMIC_DRAW, "PointLightBuffer"};
    vertexBuffer.push(gfx::frameBufferQuadVertices.data());
    frameBufferQuadVertexAttribute->attributeLocation(0, 2, offsetof(gfx::FrameBufferVertex, pos));
    frameBufferQuadVertexAttribute->attributeLocation(1, 2, offsetof(gfx::FrameBufferVertex, uv));
    frameBufferQuadVertexAttribute->bindVertexBuffer<gfx::FrameBufferVertex>(vertexBuffer);

    int selectedEntity = 0;

    double lastTime = glfwGetTime();
    float targetFPS = 30.f;

    while (!window.shouldClose()) {
        window.pollEvents();

        double currentTime = glfwGetTime(); 
        float dt = currentTime - lastTime;
        if (currentTime - lastTime < 1.f / targetFPS) {
            continue;
        } 
        lastTime = currentTime;  

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            dispatcher.post<core::ReloadShaderEvent>();
        }
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
            targetFPS = 500;
        } else {
            targetFPS = 30;
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderer::RenderContext renderContext;
        {
            renderContext.at("width") = width;
            renderContext.at("height") = height;
            renderContext.at("view") = camera.getView();
            renderContext.at("invView") = glm::inverse(camera.getView());
            renderContext.at("projection") = camera.getProjection();
            renderContext.at("invProjection") = glm::inverse(camera.getProjection());
            renderContext.at("cameraPosition") = camera.getPos();
            renderContext.at("cameraDirection") = camera.getDir();
            renderContext.at("showing") = viewPanel.getCurrentImage();

            core::DirectionalLightComponent directionalLight;
            bool found = false;
            for (auto [ent, dl] : registry.view<core::DirectionalLightComponent>().each()) {
                directionalLight = dl;
                found = true;
                break;
            }
            if (!found) {
                throw std::runtime_error("Could not find a directional light!");
            }
            renderContext.at("directionalLight") = directionalLight;
            glm::mat4 lightProjection = glm::ortho(-dlc.orthoProj, dlc.orthoProj, -dlc.orthoProj, dlc.orthoProj, dlc.near, dlc.far);  
            glm::mat4 lightView = glm::lookAt(dlc.position * dlc.multiplier, 
                                    glm::vec3( 0.0f, 0.0f,  0.0f), 
                                    glm::vec3( 0.0f, 1.0f,  0.0f));  
            glm::mat4 lightSpace = lightProjection * lightView;
            renderContext.at("lightSpace") = lightSpace;

            std::vector<core::PointLightComponent> pointLights;
            auto pointLightEntities = registry.view<core::PointLightComponent>();
            for (auto [ent, pointLight] : pointLightEntities.each()) {
                pointLights.push_back(pointLight);
            }
            if (pointLights.size() > 0) {
                pointLigthBuffer->resize(pointLights.size() * sizeof(pointLights[0]));
                pointLigthBuffer->push(pointLights.data());
            } 
            renderContext.at("pointLightBuffer") = pointLigthBuffer;
            renderContext.at("numPointLights") = int(pointLights.size());

            renderContext.at("frameBufferQuadVertexAttribute") = frameBufferQuadVertexAttribute;
        }

        mousePickingPipeline.render(registry, renderContext);

        // pipeline
        deferredPipeline.render(registry, renderContext);
        shadowPipeline.render(registry, renderContext);
        voxelPipeline.render(registry, renderContext);
        indirectPipeline.render(registry, renderContext);
        denoiserPipeline.render(registry, renderContext);
        ambientOcclusionPipeline.render(registry, renderContext);
        compositePipeline.render(registry, renderContext);

        core::startFrameImgui();

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None & ~ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = 
                                       ImGuiWindowFlags_NoDocking | 
                                       ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus |
                                       ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoDecoration;

        bool dockSpace = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        auto mainViewPort = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(mainViewPort->WorkPos);
        ImGui::SetNextWindowSize(mainViewPort->WorkSize);
        ImGui::SetNextWindowViewport(mainViewPort->ID);

        ImGui::Begin("DockSpace", &dockSpace, windowFlags);
        ImGuiID dockspaceID = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspaceID, ImGui::GetContentRegionAvail(), dockspaceFlags);
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Pipelines")) {
                for (auto& panel : pipelines) {
                    if (ImGui::MenuItem(panel->m_name.c_str(), NULL, &panel->show)) {}
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Panels")) {
                for (auto& panel : panels) {
                    if (ImGui::MenuItem(panel->m_name.c_str(), NULL, &panel->show)) {}
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        ImGui::End();
        ImGui::PopStyleVar(2);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
        ImGui::SetNextWindowClass(&window_class);
        ImGuiWindowFlags viewPortFlags = ImGuiWindowFlags_NoTitleBar |
                                         ImGuiWindowFlags_NoDecoration;
        ImGui::Begin("viewport", nullptr, viewPortFlags);
        auto vp = ImGui::GetWindowSize();
        width = vp.x;
        height = vp.y;
        camera.onUpdate(dt);
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<void *>(renderContext.at(viewPanel.getCurrentImage()).as<std::shared_ptr<gfx::Texture>>()->getID())), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::GetIO().KeysDown[ImGuiKey_E]) {
            ImVec2 wp = ImGui::GetWindowPos();
            ImVec2 mp = ImGui::GetMousePos();
            glm::ivec2 mousePos{mp.x - wp.x, mp.y - wp.y};
            auto tex = renderContext.at("texMousePicking").as<std::shared_ptr<gfx::Texture>>();
            auto info = tex->getInfo();
            mousePos.y = info.height - mousePos.y - 1;
            if (mousePos.x < info.width && mousePos.y < info.height)
                glGetTextureSubImage(renderContext.at("texMousePicking").as<std::shared_ptr<gfx::Texture>>()->getID(), 0, mousePos.x, mousePos.y, 0, 1, 1, 1, GL_RED_INTEGER, GL_INT, sizeof(int), &selectedEntity);
        } 

        if (ImGui::GetIO().KeysDown[ImGuiKey_F]) {
            camera.speedMultiplyer = 3;
        } else {
            camera.speedMultiplyer = 1;
        }

        ImGui::End();
        ImGui::PopStyleVar(2);

        for (auto panel : pipelines) {
            panel->uiPanel();
        }

        for (auto panel : panels) {
            panel->uiPanel();
        }

        ImGui::Begin("Light Edit (temporary)");
        ImGui::DragFloat3("point light position", reinterpret_cast<float *>(&(pl.position)));
        ImGui::DragFloat3("point light color", reinterpret_cast<float *>(&(pl.color)));
        ImGui::DragFloat3("point light term", reinterpret_cast<float *>(&(pl.term)));
        ImGui::Separator();
        ImGui::DragFloat3("directional light position", reinterpret_cast<float *>(&(dlc.position)), 0.01);
        ImGui::DragFloat3("directional light color", reinterpret_cast<float *>(&(dlc.color)));
        ImGui::DragFloat3("directional light ambience", reinterpret_cast<float *>(&(dlc.ambience)));
        ImGui::DragFloat3("directional light term", reinterpret_cast<float *>(&(dlc.term)));
        ImGui::DragFloat("directional light multiplier", &dlc.multiplier);
        ImGui::DragFloat("directional light orthoProj", &dlc.orthoProj);
        ImGui::DragFloat("directional light far", &dlc.far);
        ImGui::DragFloat("directional light near", &dlc.near);
        ImGui::End();

        auto mesh = registry.get<std::shared_ptr<renderer::Mesh>>(static_cast<entt::entity>(selectedEntity));
        ImGui::Begin("Selected Mesh");
        ImGui::DragFloat3("translation", reinterpret_cast<float *>(&(mesh->m_transform.translation)));
        ImGui::DragFloat3("rotation", reinterpret_cast<float *>(&(mesh->m_transform.rotation)));
        ImGui::DragFloat3("scale", reinterpret_cast<float *>(&(mesh->m_transform.scale)));
        glm::vec3 val = std::any_cast<glm::vec3>(mesh->material->get("material.emmissive"));
        ImGui::DragFloat3("emmissive", reinterpret_cast<float *>(&(val)));
        mesh->material->assign("material.emmissive", val);
        ImGui::End();

        ImGui::Begin("Debug");
        ImGui::Text("%f", ImGui::GetIO().Framerate);
        ImGui::End();

        core::endFrameImgui(window);
        
        window.updateScreen();
    }

    std::cout << glGetString(GL_RENDERER) << '\n';

}